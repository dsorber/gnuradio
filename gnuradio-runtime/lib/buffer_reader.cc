/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/integer_math.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace gr {

static long s_buffer_reader_count = 0;

buffer_reader_sptr
buffer_add_reader(buffer_sptr buf, int nzero_preload, block_sptr link, int delay)
{
    if (nzero_preload < 0)
        throw std::invalid_argument("buffer_add_reader: nzero_preload must be >= 0");

    buffer_reader_sptr r(
        new buffer_reader(buf, buf->index_sub(buf->d_write_index, nzero_preload), link));
    r->declare_sample_delay(delay);
    buf->d_readers.push_back(r.get());

    return r;
}

buffer_reader::buffer_reader(buffer_sptr buffer, unsigned int read_index, block_sptr link)
    : d_buffer(buffer),
      d_read_index(read_index),
      d_abs_read_offset(0),
      d_link(link),
      d_attr_delay(0)
{
    s_buffer_reader_count++;
}

buffer_reader::~buffer_reader()
{
    d_buffer->drop_reader(this);
    s_buffer_reader_count--;
}

void buffer_reader::declare_sample_delay(unsigned delay)
{
    d_attr_delay = delay;
    d_buffer->d_max_reader_delay = std::max(d_attr_delay, d_buffer->d_max_reader_delay);
}

unsigned buffer_reader::sample_delay() const { return d_attr_delay; }

int buffer_reader::items_available() const
{
    return d_buffer->index_sub(d_buffer->d_write_index, d_read_index);
}

const void* buffer_reader::read_pointer()
{
    return &d_buffer->d_base[d_read_index * d_buffer->d_sizeof_item];
}

void buffer_reader::update_read_pointer(int nitems)
{
    gr::thread::scoped_lock guard(*mutex());
    d_read_index = d_buffer->index_add(d_read_index, nitems);
    d_abs_read_offset += nitems;
}

void buffer_reader::get_tags_in_range(std::vector<tag_t>& v,
                                      uint64_t abs_start,
                                      uint64_t abs_end,
                                      long id)
{
    gr::thread::scoped_lock guard(*mutex());

    uint64_t lower_bound = abs_start - d_attr_delay;
    // check for underflow and if so saturate at 0
    if (lower_bound > abs_start)
        lower_bound = 0;
    uint64_t upper_bound = abs_end - d_attr_delay;
    // check for underflow and if so saturate at 0
    if (upper_bound > abs_end)
        upper_bound = 0;

    v.clear();
    std::multimap<uint64_t, tag_t>::iterator itr =
        d_buffer->get_tags_lower_bound(lower_bound);
    std::multimap<uint64_t, tag_t>::iterator itr_end =
        d_buffer->get_tags_upper_bound(upper_bound);

    uint64_t item_time;
    while (itr != itr_end) {
        item_time = (*itr).second.offset + d_attr_delay;
        if ((item_time >= abs_start) && (item_time < abs_end)) {
            std::vector<long>::iterator id_itr;
            id_itr = std::find(
                itr->second.marked_deleted.begin(), itr->second.marked_deleted.end(), id);
            // If id is not in the vector of marked blocks
            if (id_itr == itr->second.marked_deleted.end()) {
                tag_t t = (*itr).second;
                t.offset += d_attr_delay;
                v.push_back(t);
                v.back().marked_deleted.clear();
            }
        }
        itr++;
    }
}

long buffer_reader_ncurrently_allocated() { return s_buffer_reader_count; }

} /* namespace gr */