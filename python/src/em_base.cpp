#include <string>
#include <map>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>

#include "em/base/type.h"
#include "em/base/array.h"
#include "em/base/table.h"
#include "em/base/object.h"

namespace py = pybind11;

using namespace em;

//using std::map<std::string, std::string> TypeFormatMap;
//
//const TypeFormatMap& getTypeFormatMap()
//{
//
//}

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


/** Build an em::Object from a given Python object */
Object * buildObject(const py::object &pyobj)
{
    if (py::isinstance<py::str>(pyobj))
        return new Object(pyobj.cast<std::string>());
    if (py::isinstance<py::int_>(pyobj))
        return new Object(pyobj.cast<int32_t>());
    if (py::isinstance<py::float_>(pyobj))
        return new Object(pyobj.cast<float>());

    THROW_ERROR(std::string("Unsupported object type: ") +
                (std::string)py::str(pyobj.get_type()));
} // function buildObject

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
         .def(py::self == py::self)
         .def(py::self != py::self)
         .def("__hash__", &Type::getId)
         .def("getId", &Type::getId)
         .def("getName", &Type::getName)
         .def("getSize", &Type::getSize)
         .def("isPod", &Type::isPod)
         .def("isTriviallyCopyable", &Type::isTriviallyCopyable)
         .def("isNull", &Type::isNull)
         .def("toString", &Type::toString)
         .def("__repr__", &Type::toString)
         .def("__str__", &Type::toString)
         .def_static("inferFromString", (Type (*)(const std::string&))
                 &Type::inferFromString);

    m.attr("typeNull") = typeNull;
    m.attr("typeInt8") = typeInt8;
    m.attr("typeUInt8") = typeUInt8;
    m.attr("typeInt16") = typeInt16;
    m.attr("typeUInt16") = typeUInt16;
    m.attr("typeInt32") = typeInt32;
    m.attr("typeUInt32") = typeUInt32;
    m.attr("typeInt64") = typeInt64;
    m.attr("typeUInt64") = typeUInt64;

    m.attr("typeFloat") = typeFloat;
    m.attr("typeDouble") = typeDouble;
    m.attr("typeCFloat") = typeCFloat;
    m.attr("typeCDouble") = typeCDouble;

    m.attr("typeBool") = typeBool;
    m.attr("typeString") = typeString;

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
             py::arg("adim"), py::arg("type")=typeNull)
        .def("getDim", &Array::getDim);

    py::class_<Object, Type::Container>(m, "Object")
            .def(py::init<>())
            .def(py::init<const Object&>())
            .def(py::init(&buildObject))
            .def("set", [](Object &self, const py::object& pyobj)
                        {
                            Object *obj = buildObject(pyobj);
                            self = *obj;
                            delete obj;
                        })
            .def("toString", &Object::toString)
            .def("fromString", &Object::fromString)
            .def("setType", &Object::setType)
            .def("__repr__", &Object::toString)
            .def("__str__", &Object::toString)
            .def("__int__", [](const Object &self) { return (int)self; })
            .def("__float__", [](const Object &self) { return (float)self; })
            .def(py::self == py::self)
            .def(py::self != py::self);

//    py::class_<Table> table(m, "Table");
//
//    py::class_<Table::Column>(table, "Column")
//            .def(py::init<size_t, const std::string&, const Type&,
//                    const std::string&>())
//            .def(py::init<const std::string&, const Type&,
//                    const std::string&>())
//            .def("getId", &Table::Column::getId)
//            .def("getName", &Table::Column::getName)
//            .def("getType", &Table::Column::getType)
//            .def("getDescription", &Table::Column::getDescription);
//
//    py::class_<Table::Row>(table, "Row")
//            .def(py::init<>())
//            .def(py::init<const Table::Row&>())
//            .def("__getitem__", (const Object& (Table::Row::*)(size_t) const)
//                    &Table::Row::operator[])
//            .def("__getitem__", (Object& (Table::Row::*)(size_t))
//                    &Table::Row::operator[])
//            .def("__getitem__", (const Object& (Table::Row::*)(const std::string&) const)
//                    &Table::Row::operator[])
//            .def("__getitem__", (Object& (Table::Row::*)(const std::string&))
//                    &Table::Row::operator[]);

//    table.def(py::init<>())
//            .def(py::init<const Table&>());
//
//    py::class_<TableIO>(m, "TableIO")
//            .def(py::init<>())
//            .def(py::init<const std::string&>())
//            .def_static("hasImpl", &TableIO::hasImpl)
//            .def_static("registerImpl", &TableIO::registerImpl)
//            .def("open", &TableIO::open)
//            .def("close", &TableIO::close)
//            .def("read", &TableIO::read)
//            .def("write", &TableIO::write);
} // em/base sub-module definition
