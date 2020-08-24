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
#include "vmcircbuf.h"
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/integer_math.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace gr {

static long s_buffer_count = 0; // counts for debugging storage mgmt

/* ----------------------------------------------------------------------------
                      Notes on storage management

 Pretty much all the fundamental classes are now using the
 shared_ptr stuff for automatic reference counting. To ensure that
 no mistakes are made, we make the constructors for classes private,
 and then provide a free factory function that returns a smart
 pointer to the desired class.

 gr::buffer and gr::buffer_reader are no exceptions.  However, they
 both want pointers to each other, and unless we do something, we'll
 never delete any of them because of the circular structure.
 They'll always have a reference count of at least one.  We could
 use std::weak_ptr's from gr::buffer to gr::buffer_reader but that
 introduces it's own problems.  (gr::buffer_reader's destructor needs
 to call gr::buffer::drop_reader, but has no easy way to get a
 shared_ptr to itself.)

 Instead, we solve this problem by having gr::buffer hold a raw
 pointer to gr::buffer_reader in its d_reader vector.
 gr::buffer_reader's destructor calls gr::buffer::drop_reader, so
 we're never left with an dangling pointer.  gr::buffer_reader still
 has a shared_ptr to the buffer ensuring that the buffer doesn't go
 away under it.  However, when the reference count of a
 gr::buffer_reader goes to zero, we can successfully reclaim it.
 ---------------------------------------------------------------------------- */

#if 0
/*
 * Compute the minimum number of buffer items that work (i.e.,
 * address space wrap-around works).  To work is to satisfy this
 * constraint for integer buffer_size and k:
 *
 *     type_size * nitems == k * page_size
 */
static inline long minimum_buffer_items(long type_size, long page_size)
{
    return page_size / GR_GCD(type_size, page_size);
}
#endif


buffer::buffer(int nitems, size_t sizeof_item, block_sptr link)
    : d_base(0),
      d_bufsize(0),
      d_max_reader_delay(0),
      d_sizeof_item(sizeof_item),
      d_link(link),
      d_write_index(0),
      d_abs_write_offset(0),
      d_done(false),
      d_last_min_items_read(0)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer");
//    if (!allocate_buffer(nitems, sizeof_item))
//        throw std::bad_alloc();

    s_buffer_count++;
}

//buffer_sptr make_buffer(int nitems, size_t sizeof_item, block_sptr link)
//{
//    return buffer_sptr(new buffer(nitems, sizeof_item, link));
//}

buffer::~buffer()
{
    assert(d_readers.size() == 0);
    s_buffer_count--;
}

/*!
 * sets d_vmcircbuf, d_base, d_bufsize.
 * returns true iff successful.
 */
#if 0  // DBS - this needs to be pure virtual
bool buffer::allocate_buffer(int nitems, size_t sizeof_item)
{
    int orig_nitems = nitems;

    // Any buffer size we come up with must be a multiple of min_nitems.
    int granularity = gr::vmcircbuf_sysconfig::granularity();
    int min_nitems = minimum_buffer_items(sizeof_item, granularity);

    // Round-up nitems to a multiple of min_nitems.
    if (nitems % min_nitems != 0)
        nitems = ((nitems / min_nitems) + 1) * min_nitems;

    // If we rounded-up a whole bunch, give the user a heads up.
    // This only happens if sizeof_item is not a power of two.

    if (nitems > 2 * orig_nitems && nitems * (int)sizeof_item > granularity) {
        auto msg =
            str(boost::format(
                    "allocate_buffer: tried to allocate"
                    "   %d items of size %d. Due to alignment requirements"
                    "   %d were allocated.  If this isn't OK, consider padding"
                    "   your structure to a power-of-two bytes."
                    "   On this platform, our allocation granularity is %d bytes.") %
                orig_nitems % sizeof_item % nitems % granularity);
        GR_LOG_WARN(d_logger, msg.c_str());
    }

    d_bufsize = nitems;
    d_vmcircbuf.reset(gr::vmcircbuf_sysconfig::make(d_bufsize * d_sizeof_item));
    if (d_vmcircbuf == 0) {
        std::ostringstream msg;
        msg << "gr::buffer::allocate_buffer: failed to allocate buffer of size "
            << d_bufsize * d_sizeof_item / 1024 << " KB";
        GR_LOG_ERROR(d_logger, msg.str());
        return false;
    }

    d_base = (char*)d_vmcircbuf->pointer_to_first_copy();
    return true;
}
#endif

#if 0
int buffer::space_available()
{
    if (d_readers.empty())
        return d_bufsize - 1; // See comment below

    else {
        // Find out the maximum amount of data available to our readers

        int most_data = d_readers[0]->items_available();
        uint64_t min_items_read = d_readers[0]->nitems_read();
        for (size_t i = 1; i < d_readers.size(); i++) {
            most_data = std::max(most_data, d_readers[i]->items_available());
            min_items_read = std::min(min_items_read, d_readers[i]->nitems_read());
        }

        if (min_items_read != d_last_min_items_read) {
            prune_tags(d_last_min_items_read);
            d_last_min_items_read = min_items_read;
        }

        // The -1 ensures that the case d_write_index == d_read_index is
        // unambiguous.  It indicates that there is no data for the reader
        return d_bufsize - most_data - 1;
    }
}
#endif

void* buffer::write_pointer() { return &d_base[d_write_index * d_sizeof_item]; }

void buffer::update_write_pointer(int nitems)
{
    gr::thread::scoped_lock guard(*mutex());
    
    // DBS - DEBUG
    std::ostringstream msg;
    msg << "update_write_pointer -- d_write_index: " << d_write_index 
        << " -- nitems: " << nitems;
    GR_LOG_DEBUG(d_logger, msg.str());
    
    d_write_index = index_add(d_write_index, nitems);
    d_abs_write_offset += nitems;
}

void buffer::set_done(bool done)
{
    gr::thread::scoped_lock guard(*mutex());
    d_done = done;
}

void buffer::drop_reader(buffer_reader* reader)
{
    std::vector<buffer_reader*>::iterator result =
        std::find(d_readers.begin(), d_readers.end(), reader);

    if (result == d_readers.end())
        throw std::invalid_argument("buffer::drop_reader"); // we didn't find it...

    d_readers.erase(result);
}

void buffer::add_item_tag(const tag_t& tag)
{
    gr::thread::scoped_lock guard(*mutex());
    d_item_tags.insert(std::pair<uint64_t, tag_t>(tag.offset, tag));
}

void buffer::remove_item_tag(const tag_t& tag, long id)
{
    gr::thread::scoped_lock guard(*mutex());
    for (std::multimap<uint64_t, tag_t>::iterator it =
             d_item_tags.lower_bound(tag.offset);
         it != d_item_tags.upper_bound(tag.offset);
         ++it) {
        if ((*it).second == tag) {
            (*it).second.marked_deleted.push_back(id);
        }
    }
}

void buffer::prune_tags(uint64_t max_time)
{
    /* NOTE: this function _should_ lock the mutex before editing
       d_item_tags. In practice, this function is only called at
       runtime by min_available_space in block_executor.cc, which
       locks the mutex itself.

       If this function is used elsewhere, remember to lock the
       buffer's mutex al la the scoped_lock:
           gr::thread::scoped_lock guard(*mutex());
     */

    /*
      http://www.cplusplus.com/reference/map/multimap/erase/
      "Iterators, pointers and references referring to elements removed
      by the function are invalidated. All other iterators, pointers
      and references keep their validity."

      Store the iterator to be deleted in tmp; increment itr to the
      next valid iterator, then erase tmp, which now becomes invalid.
     */

    uint64_t item_time;
    std::multimap<uint64_t, tag_t>::iterator itr(d_item_tags.begin()), tmp;
    while (itr != d_item_tags.end()) {
        item_time = (*itr).second.offset;
        if (item_time + d_max_reader_delay + bufsize() < max_time) {
            tmp = itr;
            itr++;
            d_item_tags.erase(tmp);
        } else {
            // d_item_tags is a map sorted by offset, so when the if
            // condition above fails, all future tags in the map must also
            // fail. So just break here.
            break;
        }
    }
}

long buffer_ncurrently_allocated() { return s_buffer_count; }

} /* namespace gr */
