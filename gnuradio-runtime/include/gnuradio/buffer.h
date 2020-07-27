/* -*- c++ -*- */
/*
 * Copyright 2004,2009-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_H
#define INCLUDED_GR_RUNTIME_BUFFER_H

#include <gnuradio/api.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/tags.h>
#include <gnuradio/thread/thread.h>
#include <boost/weak_ptr.hpp>
#include <map>
#include <memory>

namespace gr {

class vmcircbuf;
class buffer_reader;

/*!
 * \brief Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
 *
 * The total size of the buffer will be rounded up to a system
 * dependent boundary. This is typically the system page size, but
 * under MS windows is 64KB.
 *
 * \param nitems is the minimum number of items the buffer will hold.
 * \param sizeof_item is the size of an item in bytes.
 * \param link is the block that writes to this buffer.
 */
GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                       size_t sizeof_item,
                                       block_sptr link = block_sptr());

/*!
 * \brief Single writer, multiple reader fifo.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer
{
public:
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    virtual ~buffer();

    /*!
     * \brief return number of items worth of space available for writing
     */
    int space_available();

    /*!
     * \brief return size of this buffer in items
     */
    int bufsize() const { return d_bufsize; }

    /*!
     * \brief return the base address of the buffer
     */
    const char* base() const { return static_cast<const char*>(d_base); }

    /*!
     * \brief return pointer to write buffer.
     *
     * The return value points at space that can hold at least
     * space_available() items.
     */
    void* write_pointer();

    /*!
     * \brief tell buffer that we wrote \p nitems into it
     */
    void update_write_pointer(int nitems);

    void set_done(bool done);
    bool done() const { return d_done; }

    /*!
     * \brief Return the block that writes to this buffer.
     */
    block_sptr link() { return block_sptr(d_link); }

    size_t nreaders() const { return d_readers.size(); }
    buffer_reader* reader(size_t index) { return d_readers[index]; }

    gr::thread::mutex* mutex() { return &d_mutex; }

    uint64_t nitems_written() { return d_abs_write_offset; }

    void reset_nitem_counter() { d_abs_write_offset = 0; }

    size_t get_sizeof_item() { return d_sizeof_item; }

    /*!
     * \brief  Adds a new tag to the buffer.
     *
     * \param tag        the new tag
     */
    void add_item_tag(const tag_t& tag);

    /*!
     * \brief  Removes an existing tag from the buffer.
     *
     * If no such tag is found, does nothing.
     * Note: Doesn't actually physically delete the tag, but
     * marks it as deleted. For the user, this has the same effect:
     * Any subsequent calls to get_tags_in_range() will not return
     * the tag.
     *
     * \param tag        the tag that needs to be removed
     * \param id         the unique ID of the block calling this function
     */
    void remove_item_tag(const tag_t& tag, long id);

    /*!
     * \brief  Removes all tags before \p max_time from buffer
     *
     * \param max_time        the time (item number) to trim up until.
     */
    void prune_tags(uint64_t max_time);

    std::multimap<uint64_t, tag_t>::iterator get_tags_begin()
    {
        return d_item_tags.begin();
    }
    std::multimap<uint64_t, tag_t>::iterator get_tags_end() { return d_item_tags.end(); }
    std::multimap<uint64_t, tag_t>::iterator get_tags_lower_bound(uint64_t x)
    {
        return d_item_tags.lower_bound(x);
    }
    std::multimap<uint64_t, tag_t>::iterator get_tags_upper_bound(uint64_t x)
    {
        return d_item_tags.upper_bound(x);
    }

    // -------------------------------------------------------------------------

private:
    friend class buffer_reader;
    friend GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                                  size_t sizeof_item,
                                                  block_sptr link);
    friend GR_RUNTIME_API buffer_reader_sptr buffer_add_reader(buffer_sptr buf,
                                                               int nzero_preload,
                                                               block_sptr link,
                                                               int delay);

protected:
    char* d_base;           // base address of buffer inside d_vmcircbuf.
    unsigned int d_bufsize; // in items

    // Keep track of maximum sample delay of any reader; Only prune tags past this.
    unsigned d_max_reader_delay;

private:
    std::unique_ptr<gr::vmcircbuf> d_vmcircbuf;
    size_t d_sizeof_item; // in bytes
    std::vector<buffer_reader*> d_readers;
    std::weak_ptr<block> d_link; // block that writes to this buffer

    //
    // The mutex protects d_write_index, d_abs_write_offset, d_done, d_item_tags
    // and the d_read_index's and d_abs_read_offset's in the buffer readers.
    //
    gr::thread::mutex d_mutex;
    unsigned int d_write_index;  // in items [0,d_bufsize)
    uint64_t d_abs_write_offset; // num items written since the start
    bool d_done;
    std::multimap<uint64_t, tag_t> d_item_tags;
    uint64_t d_last_min_items_read;

    unsigned index_add(unsigned a, unsigned b)
    {
        unsigned s = a + b;

        if (s >= d_bufsize)
            s -= d_bufsize;

        assert(s < d_bufsize);
        return s;
    }

    unsigned index_sub(unsigned a, unsigned b)
    {
        int s = a - b;

        if (s < 0)
            s += d_bufsize;

        assert((unsigned)s < d_bufsize);
        return s;
    }

    virtual bool allocate_buffer(int nitems, size_t sizeof_item);

    /*!
     * \brief constructor is private.  Use gr_make_buffer to create instances.
     *
     * Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
     *
     * \param nitems is the minimum number of items the buffer will hold.
     * \param sizeof_item is the size of an item in bytes.
     * \param link is the block that writes to this buffer.
     *
     * The total size of the buffer will be rounded up to a system
     * dependent boundary.  This is typically the system page size, but
     * under MS windows is 64KB.
     */
    buffer(int nitems, size_t sizeof_item, block_sptr link);

    /*!
     * \brief disassociate \p reader from this buffer
     */
    void drop_reader(buffer_reader* reader);
};

//! returns # of buffers currently allocated
GR_RUNTIME_API long buffer_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BUFFER_H */
