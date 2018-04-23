#include <string>
#include <map>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>

#include "em/base/type.h"
#include "em/base/array.h"
#include "em/image/image.h"

namespace py = pybind11;

using namespace em;

/** Return the character expected by the protocol buffer for
defining the type of the buffer, base on our own type class. */
std::string getTypeFormat(const Type &type)
{
std::cout << "Current type: " << type.getName() << std::endl;
Type t;
//#define RETURN_FORMAT_IF_TYPE(T) if (type == Type::get<T>()) return py::format_descriptor<T>::format();
#define RETURN_FORMAT_IF_TYPE(T) t = Type::get<T>(); std::cout << "Checking type: " << t.getName() << " equals: " << (type == t) << std::endl; \
if (type.getName() == t.getName()) return py::format_descriptor<T>::format();

    RETURN_FORMAT_IF_TYPE(float);
    RETURN_FORMAT_IF_TYPE(uint8_t);
    RETURN_FORMAT_IF_TYPE(int8_t);
    RETURN_FORMAT_IF_TYPE(double);
    RETURN_FORMAT_IF_TYPE(int16_t);
    RETURN_FORMAT_IF_TYPE(uint16_t);
    RETURN_FORMAT_IF_TYPE(int32_t);
    RETURN_FORMAT_IF_TYPE(uint32_t);

    THROW_ERROR("Unsupported type");
} // function getTypeFormat

void init_submodule_base(py::module &m) {

    py::class_<Error>(m, "Error")
        .def_readonly("errorCode", &Error::errorCode)
        .def_readonly("msg", &Error::msg)
        .def_readonly("fileName", &Error::fileName)
        .def_readonly("functionName", &Error::functionName)
        .def_readonly("line", &Error::line)
        .def("__repr__", &Error::toString)
        .def("__str__", &Error::toString);

    py::class_<Type>(m, "Type")
         .def(py::init<>())
         .def("getName", &Type::getName)
         .def("getSize", &Type::getSize)
         .def("isNull", &Type::isNull)
         .def("__repr__", &Type::toString)
         .def("__str__", &Type::toString);

    m.attr("typeNull") = em::typeNull;
    m.attr("typeInt8") = em::typeInt8;
    m.attr("typeUInt8") = em::typeUInt8;
    m.attr("typeInt16") = em::typeInt16;
    m.attr("typeUInt16") = em::typeUInt16;
    m.attr("typeInt32") = em::typeInt32;
    m.attr("typeUInt32") = em::typeUInt32;
    m.attr("typeFloat") = em::typeFloat;
    m.attr("typeDouble") = em::typeDouble;

    py::class_<ArrayDim>(m, "ArrayDim")
        .def(py::init<>())
        .def(py::init<const ArrayDim&>())
        .def(py::init<size_t, size_t, size_t, size_t>(),
             py::arg("x"), py::arg("y")=1, py::arg("z")=1, py::arg("n")=1)
        .def_readwrite("x", &ArrayDim::x)
        .def_readwrite("y", &ArrayDim::y)
        .def_readwrite("z", &ArrayDim::z)
        .def_readwrite("n", &ArrayDim::n)
        .def("getSize", &ArrayDim::getSize)
        .def("getItemSize", &ArrayDim::getItemSize)
        .def("getSliceSize", &ArrayDim::getSliceSize)
        .def("toString", &ArrayDim::toString)
        .def("isValidIndex", &ArrayDim::isValidIndex)
        .def("__repr__", &ArrayDim::toString)
        .def("__str__", &ArrayDim::toString)
        .def(py::self == py::self);

    py::class_<Type::Container>(m, "TypeContainer")
        .def("getType", &Type::Container::getType);

    py::class_<Array, Type::Container>(m, "Array", py::buffer_protocol())
        .def_buffer([](Array &a) -> py::buffer_info {
            auto &t = a.getType();
            auto adim = a.getDim();
            auto size = t.getSize();

            if (adim.z > 1) // 3D volume
                return py::buffer_info(
                    a.getData(),                      /* Pointer to buffer */
                    size,                             /* Size of one scalar */
                    getTypeFormat(t),                 /* Python struct-style format descriptor */
                    3,                                /* Number of dimensions */
                    { adim.z, adim.y, adim.x },       /* Buffer dimensions */
                    { size * adim.getSliceSize(),     /* Strides (in bytes) for each index */
                      size * adim.x, size }
                );

            if (adim.y > 1) // 2D image
                return py::buffer_info(
                    a.getData(),                      /* Pointer to buffer */
                    size,                             /* Size of one scalar */
                    getTypeFormat(t),                 /* Python struct-style format descriptor */
                    2,                                /* Number of dimensions */
                    { adim.y, adim.x },               /* Buffer dimensions */
                    { size * adim.x, size }           /* Strides (in bytes) for each index */
                );

            // 1D Array
            return py::buffer_info(
                    a.getData(),                      /* Pointer to buffer */
                    size,                             /* Size of one scalar */
                    getTypeFormat(t),                 /* Python struct-style format descriptor */
                    1,                                /* Number of dimensions */
                    { adim.x },                       /* Buffer dimensions */
                    { size }                          /* Strides (in bytes) for each index */
                );
        })
        .def(py::init<>())
        .def(py::init<const ArrayDim&, const Type &>())
        .def(py::init<const Array&>())
        .def("getAlias", &Array::getAlias,
             py::arg("index")=0)
        .def("resize", (void (Array::*)(const Array&)) &Array::resize)
        .def("resize", (void (Array::*)(const ArrayDim&, const Type & )) &Array::resize,
             py::arg("adim"), py::arg("type")=em::typeNull)
        .def("getDim", &Array::getDim);


} // em/base sub-module definition
