/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(top_block.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(96ae1e4a3aed7d59ef7e8c596e83c07e)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/runtime_types.h>
#include <gnuradio/top_block.h>
// pydoc.h is automatically generated in the build directory
#include <top_block_pydoc.h>

#define GR_PYTHON_BLOCKING_CODE(code)    \
    {                                    \
        PyThreadState* _save;            \
        _save = PyEval_SaveThread();     \
        try {                            \
            code                         \
        } catch (...) {                  \
            PyEval_RestoreThread(_save); \
            throw;                       \
        }                                \
        PyEval_RestoreThread(_save);     \
    }


void top_block_run_unlocked(gr::top_block_sptr r) noexcept(false)
{
    GR_PYTHON_BLOCKING_CODE(r->run();)
}

void top_block_start_unlocked(gr::top_block_sptr r, int max_noutput_items) noexcept(false)
{
    GR_PYTHON_BLOCKING_CODE(r->start(max_noutput_items);)
}

void top_block_wait_unlocked(gr::top_block_sptr r) noexcept(false)
{
    GR_PYTHON_BLOCKING_CODE(r->wait();)
}

void top_block_stop_unlocked(gr::top_block_sptr r) noexcept(false)
{
    GR_PYTHON_BLOCKING_CODE(r->stop();)
}

void top_block_unlock_unlocked(gr::top_block_sptr r) noexcept(false)
{
    GR_PYTHON_BLOCKING_CODE(r->unlock();)
}


void bind_top_block(py::module& m)
{
    using top_block = gr::top_block;


    py::class_<top_block, gr::hier_block2, gr::basic_block, std::shared_ptr<top_block>>(
        m, "top_block_pb", D(top_block))


        .def(py::init(&gr::make_top_block),
             py::arg("name"),
             py::arg("catch_exceptions") = false)

        .def("run",
             &top_block::run,
             py::arg("max_noutput_items") = 100000000,
             D(top_block, run))


        .def("start",
             &top_block::start,
             py::arg("max_noutput_items") = 100000000,
             D(top_block, start))


        .def("stop", &top_block::stop, D(top_block, stop))


        .def("wait", &top_block::wait, D(top_block, wait))


        .def("lock", &top_block::lock, D(top_block, lock))


        .def("unlock", &top_block::unlock, D(top_block, unlock))


        .def("edge_list", &top_block::edge_list, D(top_block, edge_list))


        .def("msg_edge_list", &top_block::msg_edge_list, D(top_block, msg_edge_list))


        .def("dump", &top_block::dump, D(top_block, dump))


        .def("max_noutput_items",
             &top_block::max_noutput_items,
             D(top_block, max_noutput_items))


        .def("set_max_noutput_items",
             &top_block::set_max_noutput_items,
             py::arg("nmax"),
             D(top_block, set_max_noutput_items))


        .def("to_top_block", &top_block::to_top_block, D(top_block, to_top_block))


        .def("setup_rpc", &top_block::setup_rpc, D(top_block, setup_rpc))

        ;


    m.def("make_top_block",
          &::gr::make_top_block,
          py::arg("name"),
          py::arg("catch_exceptions") = true,
          D(make_top_block));


    m.def("cast_to_top_block_sptr",
          &::gr::cast_to_top_block_sptr,
          py::arg("block"),
          D(cast_to_top_block_sptr));


    m.def("top_block_run_unlocked", &top_block_run_unlocked);
    m.def("top_block_start_unlocked", &top_block_start_unlocked);
    m.def("top_block_wait_unlocked", &top_block_wait_unlocked);
    m.def("top_block_stop_unlocked", &top_block_stop_unlocked);
    m.def("top_block_unlock_unlocked", &top_block_unlock_unlocked);
}
