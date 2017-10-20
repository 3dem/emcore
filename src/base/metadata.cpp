//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include "em/base/metadata.h"

using namespace em;


// ========================== Column Implementation ============================

const size_t Column::NO_ID = -1;

Column::Column(size_t id, const std::string &name, ConstTypePtr type,
                   const std::string &description):
                   id(id), name(name), type(type), descr(description)
{

} // Ctor Column

size_t Column::getId() const { return id; }

std::string Column::getName() const { return name; }

ConstTypePtr Column::getType() const { return type; }

std::string Column::getDescription() const { return descr; }


// ======================= ColumnIndex Implementation ==========================

const size_t ColumnIndex::NO_INDEX = -1;

size_t ColumnIndex::addColumn(const Column &column)
{
    size_t index = columns.size();
    columns.push_back(column);
    return colStrMap[column.getName()] = colIntMap[column.getId()] = index;
} // function ColumnIndex.addColumn

size_t ColumnIndex::operator[](size_t columnId)
{
    auto it = colIntMap.find(columnId);
    return it != colIntMap.end() ? colIntMap[columnId] : NO_INDEX;
} // function ColumnIndex.operator[string]

size_t ColumnIndex::operator[](const std::string &columnName)
{
    auto it = colStrMap.find(columnName);
    return it != colStrMap.end() ? colStrMap[columnName] : NO_INDEX;
} // function ColumnIndex.operator[string]

size_t ColumnIndex::size() const { return columns.size(); }

const ColumnVector& ColumnIndex::getColumns() const { return columns; }


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
        std::cerr << "RowImpl constructor..." << std::endl;
    } // Ctor Table::RowImpl
}; // class Table::RowImpl

class Table::TableImpl
{
public:
    ColumnIndex colIndex;

    std::vector<Table::Row> rows;
}; // class Table::TableImpl

Table::Row::Row(RowImpl *rowImpl): impl(rowImpl)
{
    std::cerr << "Row constructor..." << std::endl;
} // Ctor

Table::Row::Row(const Row &other)
{
    std::cerr << "Row copy CTOR " << std::endl;
    impl = new RowImpl();
    *this = other;
} // Copy ctor

Table::Row& Table::Row::operator=(const Row &other)
{
    std::cerr << "Row operator= " << std::endl;
    *impl = *(other.impl);
    return *this;
} // Copy Ctor Table::Row

Table::Row::~Row()
{
    std::cerr << "Destroying Row: " << std::endl <<
              "   this: " << this << std::endl <<
              "   impl: " << impl << std::endl;
    delete impl;
} // Dtor Table::Row

Object& Table::Row::operator[](size_t columnId)
{
    size_t index = impl->parent->impl->colIndex[columnId];
    return impl->objects[index];
} // function Table::Row.operator[size_t]

Object& Table::Row::operator[](const std::string &columnName)
{
    size_t index = impl->parent->impl->colIndex[columnName];
    return impl->objects[index];
} // function Table::Row.operator[string]

void Table::Row::toStream(std::ostream &ostream) const
{
    auto& columns = impl->parent->impl->colIndex.getColumns();
    for (size_t i = 0; i < columns.size(); ++i)
    {
        ostream << columns[i].getName() << ": ";
        impl->objects[i].toStream(ostream);
        ostream << std::endl;
    }
} // function Table::Row.toStream

std::ostream& operator<< (std::ostream &ostream, const Table::Row &row)
{
    row.toStream(ostream);
    return ostream;
}

Table::Table(const ColumnVector &columns)
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
    std::cerr << "Number of columns: " << impl->colIndex.size() << std::endl;
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