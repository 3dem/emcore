#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>
#include "em/image/image.h"

namespace py = pybind11;

using namespace em;


void init_submodule_image(py::module &m) {

    py::class_<ImageLocation>(m, "ImageLocation")
//        .def_readonly_static("FIRST", 1)
//        .def_readonly_static("ALL", 0)
        .def(py::init<>())
        .def(py::init<const std::string&, size_t>(),
            py::arg("path"), py::arg("index")=1)
        .def_readwrite("path", &ImageLocation::path)
        .def_readwrite("index", &ImageLocation::index)
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<Image, Array>(m, "Image")
        .def(py::init<>())
        .def(py::init<>())
        .def(py::init<const ArrayDim&, const Type &>())
        .def(py::init<const Image&>())
        .def("read", &Image::read)
        .def("write", &Image::write);

    py::class_<ImageIO>(m, "ImageIO")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def_static("hasImpl", &ImageIO::hasImpl)
        .def("getDim", &ImageIO::getDim)
        .def("open", &ImageIO::open)
        .def("close", &ImageIO::close)
        .def("createFile", &ImageIO::createFile)
        .def("expandFile", &ImageIO::expandFile)
        .def("read", &ImageIO::read)
        .def("write", &ImageIO::write);

} // em/image sub-module definition
