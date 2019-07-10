#include <string>
#include <map>
#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

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
#define TYPE_FORMAT(t) {Type::get<t>(), py::format_descriptor<t>::format()}

    static const TypeStringMap typeFormatMap = {
            TYPE_FORMAT(int8_t),
            TYPE_FORMAT(uint8_t),
            TYPE_FORMAT(int16_t),
            TYPE_FORMAT(uint16_t),
            TYPE_FORMAT(int32_t),
            TYPE_FORMAT(uint32_t),
            TYPE_FORMAT(float),
            TYPE_FORMAT(double)
    };

    auto it = typeFormatMap.find(type);
    if (it != typeFormatMap.end())
        return it->second;

    std::cout << "Missing type: " << type.getId() << std::endl;

    THROW_ERROR(std::string("Unsupported type ") + type.toString());
} // function getTypeFormat

/** Set the value of a given em::Object from a python object */
void setObjectValue(const py::object &pyobj, Object &obj)
{
    if (py::isinstance<py::str>(pyobj))
        obj = pyobj.cast<std::string>();
    else if (py::isinstance<py::int_>(pyobj))
        obj = pyobj.cast<int32_t>();
    else if (py::isinstance<py::float_>(pyobj))
        obj = pyobj.cast<float>();
    else
        THROW_ERROR(std::string("Unsupported object type: ") +
                   (std::string)py::str(pyobj.get_type()));
} // function buildObject

/** Build an em::Object from a given Python object */
std::unique_ptr<Object> buildObject(const py::object &pyobj)
{
    auto obj = new Object();
    setObjectValue(pyobj, *obj);
    return std::unique_ptr<Object>(obj);
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
    m.attr("typeSizeT") = typeSizeT;

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

    py::class_<TypedContainer>(m, "TypedContainer")
        .def("getType", &TypedContainer::getType)
        .def("getDataSize", &TypedContainer::getDataSize);

    py::class_<Array, TypedContainer>(m, "Array", py::buffer_protocol())
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
        .def("getView", (Array (Array::*)(size_t)) &Array::getView,
             py::arg("index")=0)
        .def("resize", (void (Array::*)(const Array&)) &Array::resize)
        .def("resize", (void (Array::*)(const ArrayDim&, const Type & )) &Array::resize,
             py::arg("adim"), py::arg("type")=typeNull)
        .def("getDim", &Array::getDim);

    py::class_<Object, TypedContainer>(m, "Object")
            .def(py::init<>())
            .def(py::init<const Object&>())
            .def(py::init(&buildObject))
            .def("set", (void (Object::*)(const Object&)) &Object::set)
            .def("set", [](Object &self, const py::object& pyobj)
                        {
                            auto obj = buildObject(pyobj);
                            self.set(*obj);
                        })
            .def("toString", &Object::toString)
            .def("fromString", &Object::fromString)
            .def("setType", &Object::setType)
            .def("__repr__", &Object::toString)
            .def("__str__", &Object::toString)
            .def("__int__", [](const Object &self) { return self.get<int>(); })
            .def("__float__", [](const Object &self) { return self.get<float>(); })
            .def(py::self == py::self)
            .def("__eq__", [](Object &self, const py::object& pyobj)
            {
                auto obj = buildObject(pyobj);
                return self == *obj;
            })
            .def(py::self != py::self);

    py::class_<File> file(m, "File");
    py::enum_<File::Mode>(file, "Mode")
            .value("READ_ONLY", File::Mode::READ_ONLY)
            .value("READ_WRITE", File::Mode::READ_WRITE)
            .value("TRUNCATE", File::Mode::TRUNCATE)
            .export_values();

    py::class_<Table> table(m, "Table");

    py::class_<Table::Column>(table, "Column")
            .def_readonly_static("NO_ID", &Table::Column::NO_ID)
            .def_readonly_static("NO_INDEX", &Table::Column::NO_INDEX)
            .def(py::init<size_t, const std::string&, const Type&, const std::string&>(),
                 py::arg("id"), py::arg("name"), py::arg("type"), py::arg("description")="")
            .def(py::init<const std::string&, const Type&, const std::string&>(),
                 py::arg("name"), py::arg("type"), py::arg("description")="")
            .def("getId", &Table::Column::getId)
            .def("getName", &Table::Column::getName)
            .def("getType", &Table::Column::getType)
            .def("getDescription", &Table::Column::getDescription)
            .def("toString", &Table::Column::toString)
            .def("__repr__", &Table::Column::toString)
            .def("__str__", &Table::Column::toString);

    py::class_<Table::Row>(table, "Row")
            .def(py::init<>())
            .def(py::init<const Table::Row&>())
            .def("__getitem__", (const Object& (Table::Row::*)(size_t) const)
                    &Table::Row::operator[])
            .def("__getitem__", (const Object& (Table::Row::*)(const std::string&) const)
                    &Table::Row::operator[])
            .def("__setitem__", [](Table::Row& self, size_t colId, py::object &pyobj)
                                {
                                    setObjectValue(pyobj, self[colId]);
                                })
            .def("__setitem__", [](Table::Row& self, const std::string &colName, py::object &pyobj)
                                {
                                    setObjectValue(pyobj, self[colName]);
                                })
            .def("toString", &Table::Row::toString)
            .def("__repr__", &Table::Row::toString)
            .def("__str__", &Table::Row::toString);

    table.def(py::init<>())
            .def(py::init<>())
            .def(py::init<const Table&>())
            .def(py::init<const std::vector<Table::Column>&>())
            .def("clear", &Table::clear)
            .def("getSize", &Table::getSize)
            .def("isEmpty", &Table::isEmpty)
            .def("getIndex", (size_t (Table::*)(size_t)) &Table::getIndex)
            .def("getIndex", (size_t (Table::*)(const std::string&)) &Table::getIndex)
            .def("getColumn", (const Table::Column& (Table::*)(size_t)) &Table::getColumn)
            .def("getColumn", (const Table::Column& (Table::*)(const std::string&)) &Table::getColumn)
            .def("getColumnByIndex", &Table::getColumnByIndex)
            .def("getColumnsSize", &Table::getColumnsSize)
            .def("addColumn", (size_t (Table::*)(const Table::Column&)) &Table::addColumn)
            .def("addColumn", (size_t (Table::*)(const Table::Column&, const Object&)) &Table::addColumn)
            .def("insertColumn", (size_t (Table::*)(const Table::Column&, size_t)) &Table::insertColumn)
            .def("insertColumn", (size_t (Table::*)(const Table::Column&, size_t, const Object&)) &Table::insertColumn)
            .def("removeColumn", (void (Table::*)(size_t)) &Table::removeColumn)
            .def("removeColumn", (void (Table::*)(const std::string&)) &Table::removeColumn)
            .def("iterColumns", [](const Table& table)
                                    { return py::make_iterator(table.cbegin_cols(), table.cend_cols()); })
            .def("createRow", &Table::createRow)
            .def("addRow", &Table::addRow)
            .def("sort", &Table::sort)
            .def("read", (void (Table::*)(const std::string&, const std::string&)) &Table::read)
            .def("read", (void (Table::*)(const std::string&)) &Table::read)
            .def("__getitem__", (Table::Row& (Table::*)(size_t)) &Table::operator[],
                 py::return_value_policy::reference)
            .def("toString", &Table::toString)
            .def("__repr__", &Table::toString)
            .def("__str__", &Table::toString)
            .def("__iter__", [](const Table& table)
                                {
                                    return py::make_iterator(table.cbegin(), table.cend());
                                });

//            .def("insertRow", &Table::insertRow)
//            .def("deleteRow", &Table::deleteRow)
//            .def("deleteRows", &Table::deleteRows)
//            .def("updateRow", &Table::updateRow)
//            .def("updateRows", &Table::updateRows);

    py::class_<TableIO>(m, "TableIO")
            .def(py::init<>())
            .def(py::init<const std::string&>())
            .def_static("hasImpl", &TableIO::hasImpl)
            .def_static("registerImpl", &TableIO::registerImpl)
            .def("open", &TableIO::open,
                 py::arg("filename"), py::arg("mode")=File::Mode::READ_ONLY)
            .def("close", &TableIO::close)
            .def("getTableNames", &TableIO::getTableNames)
            .def("read", &TableIO::read);
            //.def("write", &TableIO::write);
} // em/base sub-module definition
