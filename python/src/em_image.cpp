#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

#include "em/base/image.h"

namespace py = pybind11;

using namespace em;


void init_submodule_image(py::module &m) {

    py::class_<ImageLocation> imgLoc(m, "ImageLocation");
    imgLoc.def(py::init<>())
          .def(py::init<const std::string&, size_t>(),
               py::arg("path"), py::arg("index")=1)
          .def_readwrite("path", &ImageLocation::path)
          .def_readwrite("index", &ImageLocation::index)
          .def(py::self == py::self)
          .def(py::self != py::self);

    py::enum_<ImageLocation::Index>(imgLoc, "Index")
            .value("NONE", ImageLocation::Index::NONE)
            .value("ALL", ImageLocation::Index::ALL)
            .value("FIRST", ImageLocation::Index::FIRST)
            .export_values();

    py::class_<Image, Array>(m, "Image")
        .def(py::init<>())
        .def(py::init<>())
        .def(py::init<const ArrayDim&, const Type &>())
        .def(py::init<const Image&>())
        .def("read", &Image::read)
        .def("write", &Image::write);

    py::class_<ImageFile>(m, "ImageFile")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def_static("hasImpl", &ImageFile::hasImpl)
        .def_static("getImplTypes", &ImageFile::getImplTypes)
        .def_static("getFormatTypes", &ImageFile::getFormatTypes)
        .def("getDim", &ImageFile::getDim)
        .def("getType", &ImageFile::getType)
        .def("open", &ImageFile::open)
        .def("close", &ImageFile::close)
        .def("createEmpty", &ImageFile::createEmpty)
        .def("expand", &ImageFile::expand)
        .def("read", &ImageFile::read)
        .def("write", &ImageFile::write);

} // em/image sub-module definition
