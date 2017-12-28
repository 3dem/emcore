//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include "em/base/metadata.h"

using namespace em;


// ========================== Column Implementation ============================

const size_t ColumnMap::NO_ID = 0;
const size_t ColumnMap::NO_INDEX = -1;

ColumnMap::Column::Column(size_t id, const std::string &name, ConstTypePtr type,
                   const std::string &description):
                   id(id), name(name), type(type), descr(description) {}

ColumnMap::Column::Column(const std::string &name, ConstTypePtr type,
                          const std::string &description):
                    Column(NO_ID, name, type, description) {}

size_t ColumnMap::Column::getId() const { return id; }

std::string ColumnMap::Column::getName() const { return name; }

ConstTypePtr ColumnMap::Column::getType() const { return type; }

std::string ColumnMap::Column::getDescription() const { return descr; }


// ======================= ColumnMap Implementation ==========================

class ColumnMap::Impl
{
public:
    ColumnVector columns;
    std::map<size_t, size_t> colIntMap;
    std::map<std::string, size_t> colStrMap;
    size_t maxId = ColumnMap::NO_ID; // Keep track of the biggest ID

    inline size_t getIndex(size_t columnId) const
    {
        return colIntMap.find(columnId) != colIntMap.end() ?
               colIntMap.at(columnId) : NO_INDEX;
    }

    inline size_t getIndex(const std::string &columnName) const
    {
        return colStrMap.find(columnName) != colStrMap.end() ?
               colStrMap.at(columnName) : NO_INDEX;
    }

    inline const ColumnMap::Column& getColumn(size_t index) const
    {
        return columns[index];
    }
}; // class ColumnMap::Impl

ColumnMap::ColumnMap() { impl = new Impl(); }

ColumnMap::ColumnMap(std::initializer_list<Column> list): ColumnMap()
{
    for (auto& column: list)
        addColumn(column);
} // Ctor from list of Columns

ColumnMap::~ColumnMap() { delete impl; }

size_t ColumnMap::addColumn(const Column &column)
{
    size_t index = impl->columns.size();

    auto colId = column.getId();
    auto colName = column.getName();

    if (colId == NO_ID) // Find a new ID
        colId = ++(impl->maxId); // increment maxId and assign it to colId
    else
        impl->maxId = std::max(impl->maxId, colId);

    impl->columns.emplace_back(colId, colName, column.getType(),
                               column.getDescription());

    return impl->colStrMap[colName] = impl->colIntMap[colId] = index;
} // function ColumnMap.addColumn

const ColumnMap::Column& ColumnMap::getColumn(size_t columnId)
{
    return impl->getColumn(impl->getIndex(columnId));
} // function ColumnMap.getColumn(size_t)

const ColumnMap::Column& ColumnMap::getColumn(const std::string &columnName)
{
    return impl->getColumn(impl->getIndex(columnName));
} // function ColumnMap.getColumn(std::string)

size_t ColumnMap::getIndex(size_t columnId)
{
    return impl->getIndex(columnId);
} // function ColumnMap.getColumn(size_t)

size_t ColumnMap::getIndex(const std::string &columnName)
{
    return impl->getIndex(columnName);
} // function ColumnMap.getColumn(std::string)

const ColumnMap::Column& ColumnMap::operator[](size_t index)
{
    return impl->getColumn(index);
} // function ColumnMap.operator[string]

size_t ColumnMap::size() const { return impl->columns.size(); }

ColumnMap::iterator ColumnMap::begin()
{
    return impl->columns.begin();
}

ColumnMap::iterator ColumnMap::end()
{
    return impl->columns.end();
}


// ========================== Table Implementation =============================

class Table::RowImpl
{
public:
    const Table * parent;
    std::vector<Object> objects;

    /** Default empty constructor */
    RowImpl() = default;

    /** Constructor of RowImpl. Receives the parent Table pointer and
     * the number of objects.
     */
    RowImpl(const Table * parent, size_t n): parent(parent)
    {
        // TODO: The Table could be implemented in the future to
        // contain a vector with all objects. So the row will only need
        // to point to the first object of this row.
        objects.resize(n); // Allocate space for N objects
    } // Ctor Table::RowImpl
}; // class Table::RowImpl

class Table::TableImpl
{
public:
    ColumnMap colIndex;

    std::vector<Table::Row> rows;
}; // class Table::TableImpl

Table::Row::Row(RowImpl *rowImpl): impl(rowImpl) {}

Table::Row::Row(const Row &other)
{
    impl = new RowImpl();
    *this = other;
} // Copy ctor

Table::Row& Table::Row::operator=(const Row &other)
{
    *impl = *(other.impl);
    return *this;
} // Copy Ctor Table::Row

Table::Row::~Row() { delete impl; }

Object& Table::Row::operator[](size_t columnId)
{
    size_t index = impl->parent->impl->colIndex.getIndex(columnId);
    return impl->objects[index];
} // function Table::Row.operator[size_t]

Object& Table::Row::operator[](const std::string &columnName)
{
    size_t index = impl->parent->impl->colIndex.getIndex(columnName);
    return impl->objects[index];
} // function Table::Row.operator[string]

void Table::Row::toStream(std::ostream &ostream) const
{
    size_t i = 0;
    for (auto& column: impl->parent->impl->colIndex)
    {
        ostream << column.getName() << ": ";
        impl->objects[i++].toStream(ostream);
        ostream << "\t";
    }
} // function Table::Row.toStream

std::ostream& operator<< (std::ostream &ostream, const Table::Row &row)
{
    row.toStream(ostream);
    return ostream;
}

Table::Table(std::initializer_list<ColumnMap::Column> columns)
{
    impl = new TableImpl();

    for (auto& col: columns)
        impl->colIndex.addColumn(col);
} // Ctor Table

Table::~Table()
{
    delete impl;
} // Dtor ~Table

Table::Row Table::createRow() const
{
    RowImpl *rowImpl = new RowImpl(this, impl->colIndex.size());
    return Row(rowImpl);
} // function Table.createRow

bool Table::addRow(const Row &row)
{
    ASSERT_ERROR(row.impl->parent != this,
                 "This row has not been created by this Table. ");

    impl->rows.push_back(row);

    return true;
} // function Table.addRow

Table::iterator Table::begin()
{
    return impl->rows.begin();
}

Table::iterator Table::end()
{
    return impl->rows.end();
}
