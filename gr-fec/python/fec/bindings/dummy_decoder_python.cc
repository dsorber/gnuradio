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
/* BINDTOOL_HEADER_FILE(dummy_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a170def333500e9e59e873037b63c464)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec/dummy_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <dummy_decoder_pydoc.h>

void bind_dummy_decoder(py::module& m)
{


    py::module m_code = m.def_submodule("code");

    using dummy_decoder = ::gr::fec::code::dummy_decoder;


    py::class_<dummy_decoder, gr::fec::generic_decoder, std::shared_ptr<dummy_decoder>>(
        m_code, "dummy_decoder", D(code, dummy_decoder))

        .def_static("make",
                    &dummy_decoder::make,
                    py::arg("frame_size"),
                    D(code, dummy_decoder, make))


        .def("set_frame_size",
             &dummy_decoder::set_frame_size,
             py::arg("frame_size"),
             D(code, dummy_decoder, set_frame_size))


        .def("rate", &dummy_decoder::rate, D(code, dummy_decoder, rate))

        ;
}
