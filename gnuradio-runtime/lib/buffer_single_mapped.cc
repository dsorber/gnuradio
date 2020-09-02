/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
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
#include <gnuradio/buffer_single_mapped.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/integer_math.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace gr {

buffer_single_mapped::buffer_single_mapped(int nitems, size_t sizeof_item, block_sptr link)
    : buffer(nitems, sizeof_item, link)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_single_mapped");
    if (!allocate_buffer(nitems, sizeof_item))
        throw std::bad_alloc();
    
    GR_LOG_DEBUG(d_logger, "buffer_single_mapped constructor");
}

#ifdef SINGLE_MAPPED
buffer_sptr make_buffer(int nitems, size_t sizeof_item, block_sptr link)
{
    // DBS - DEBUG
    gr::logger_ptr logger;
    gr::logger_ptr debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "buffer_single_mapped");
    std::ostringstream msg;
    msg << "make_buffer() called  nitems: " << nitems 
        << " -- sizeof_item: " << sizeof_item;
    GR_LOG_DEBUG(logger, msg.str());
    
    return buffer_sptr(new buffer_single_mapped(nitems, sizeof_item, link));
}
#endif

buffer_single_mapped::~buffer_single_mapped()
{
}

/*!
 * sets d_vmcircbuf, d_base, d_bufsize.
 * returns true iff successful.
 */
bool buffer_single_mapped::allocate_buffer(int nitems, size_t sizeof_item)
{
    d_buffer.reset(new char[nitems * sizeof_item]);
    
    d_base = d_buffer.get();
    d_bufsize = nitems;
    
    return true;
}

int buffer_single_mapped::space_available()
{
    if (d_readers.empty())
        return d_bufsize - 1; // See comment below

    else {
        // Find the reader with the smallest read index
        unsigned min_read_index = d_readers[0]->d_read_index;
        buffer_reader* min_idx_reader = d_readers[0];
        uint64_t min_items_read = d_readers[0]->nitems_read();
        for (size_t idx = 1; idx < d_readers.size(); ++idx) {
            if (d_readers[idx]->d_read_index < min_read_index) {
                min_read_index = d_readers[idx]->d_read_index;
                min_idx_reader = d_readers[idx];
            }
            min_items_read = std::min(min_items_read, d_readers[idx]->nitems_read());
        }
        
        // For single mapped buffer there is no wrapping beyond the end of the
        // buffer
        int thecase  = -1;  // REMOVE ME - just for debug
        int space = d_bufsize - d_write_index;
        if (min_read_index == d_write_index)
        {
            thecase = 1; 
            
            // If the (min) read index and write index are equal then the buffer
            // is either completely empty or completely full depending on if 
            // the number of items read matches the number written
            if (min_idx_reader->nitems_read() != nitems_written())
            {
                thecase = 2; 
                space = 0;
            }
        }
        else if (min_read_index > d_write_index)
        {
            thecase = 3;
            space = min_read_index - d_write_index;
        }
        
        if (min_items_read != d_last_min_items_read) {
            prune_tags(d_last_min_items_read);
            d_last_min_items_read = min_items_read;
        }
        
        // DBS - DEBUG
        std::ostringstream msg;
        std::string problem;
        msg << "[" << this << "] " 
            << "space_available() called  (case: " << thecase 
            << ")  d_write_index: "  << d_write_index 
            << " -- min_read_index: " << min_read_index 
            << " -- space: " << space;
        GR_LOG_DEBUG(d_logger, msg.str());
        
        assert(space > 0);
        return space;
    }
}

} /* namespace gr */