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
/* BINDTOOL_HEADER_FILE(fft.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(ebbb274b1b433908749981712d96b3e9)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fft/fft.h>
// pydoc.h is automatically generated in the build directory
#include <fft_pydoc.h>

void bind_fft(py::module& m)
{

    using planner = ::gr::fft::planner;
    using fft_complex = ::gr::fft::fft_complex;
    using fft_real_fwd = ::gr::fft::fft_real_fwd;
    using fft_real_rev = ::gr::fft::fft_real_rev;


    py::class_<planner, std::shared_ptr<planner>>(m, "planner", D(planner))

        .def(py::init<>(), D(planner, planner, 0))
        .def(py::init<gr::fft::planner const&>(), py::arg("arg0"), D(planner, planner, 1))


        .def_static("mutex", &planner::mutex, D(planner, mutex))

        ;


    py::class_<fft_complex, std::shared_ptr<fft_complex>>(
        m, "fft_complex", D(fft_complex))

        .def(py::init<int, bool, int>(),
             py::arg("fft_size"),
             py::arg("forward") = true,
             py::arg("nthreads") = 1,
             D(fft_complex, fft_complex))


        .def("get_inbuf", &fft_complex::get_inbuf, D(fft_complex, get_inbuf))


        .def("get_outbuf", &fft_complex::get_outbuf, D(fft_complex, get_outbuf))


        .def("inbuf_length", &fft_complex::inbuf_length, D(fft_complex, inbuf_length))


        .def("outbuf_length", &fft_complex::outbuf_length, D(fft_complex, outbuf_length))


        .def("set_nthreads",
             &fft_complex::set_nthreads,
             py::arg("n"),
             D(fft_complex, set_nthreads))


        .def("nthreads", &fft_complex::nthreads, D(fft_complex, nthreads))


        .def("execute", &fft_complex::execute, D(fft_complex, execute))

        ;


    py::class_<fft_real_fwd, std::shared_ptr<fft_real_fwd>>(
        m, "fft_real_fwd", D(fft_real_fwd))

        .def(py::init<int, int>(),
             py::arg("fft_size"),
             py::arg("nthreads") = 1,
             D(fft_real_fwd, fft_real_fwd))


        .def("get_inbuf", &fft_real_fwd::get_inbuf, D(fft_real_fwd, get_inbuf))


        .def("get_outbuf", &fft_real_fwd::get_outbuf, D(fft_real_fwd, get_outbuf))


        .def("inbuf_length", &fft_real_fwd::inbuf_length, D(fft_real_fwd, inbuf_length))


        .def(
            "outbuf_length", &fft_real_fwd::outbuf_length, D(fft_real_fwd, outbuf_length))


        .def("set_nthreads",
             &fft_real_fwd::set_nthreads,
             py::arg("n"),
             D(fft_real_fwd, set_nthreads))


        .def("nthreads", &fft_real_fwd::nthreads, D(fft_real_fwd, nthreads))


        .def("execute", &fft_real_fwd::execute, D(fft_real_fwd, execute))

        ;


    py::class_<fft_real_rev, std::shared_ptr<fft_real_rev>>(
        m, "fft_real_rev", D(fft_real_rev))

        .def(py::init<int, int>(),
             py::arg("fft_size"),
             py::arg("nthreads") = 1,
             D(fft_real_rev, fft_real_rev))


        .def("get_inbuf", &fft_real_rev::get_inbuf, D(fft_real_rev, get_inbuf))


        .def("get_outbuf", &fft_real_rev::get_outbuf, D(fft_real_rev, get_outbuf))


        .def("inbuf_length", &fft_real_rev::inbuf_length, D(fft_real_rev, inbuf_length))


        .def(
            "outbuf_length", &fft_real_rev::outbuf_length, D(fft_real_rev, outbuf_length))


        .def("set_nthreads",
             &fft_real_rev::set_nthreads,
             py::arg("n"),
             D(fft_real_rev, set_nthreads))


        .def("nthreads", &fft_real_rev::nthreads, D(fft_real_rev, nthreads))


        .def("execute", &fft_real_rev::execute, D(fft_real_rev, execute))

        ;
}
