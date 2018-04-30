//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <iostream>
#include <fstream> // the good one
#include <sstream>

#include "em/base/string.h"
#include "em/base/registry.h"
#include "em/base/metadata.h"
#include "em/base/metadata_priv.h"
#include "em/os/filesystem.h"


using namespace em;
using Column = Table::Column;
using Row = Table::Row;


// ========================== Column Implementation ============================

const size_t Table::Column::NO_ID = 0;
const size_t Table::Column::NO_INDEX = -1;

Table::Column::Column(size_t id, const std::string &name, const Type & type,
                   const std::string &description):
                   id(id), name(name), type(type), descr(description) {}

Table::Column::Column(const std::string &name, const Type & type,
                          const std::string &description):
                    Column(NO_ID, name, type, description) {}

size_t Table::Column::getId() const { return id; }

std::string Table::Column::getName() const { return name; }

const Type & Table::Column::getType() const { return type; }

std::string Table::Column::getDescription() const { return descr; }


// ========================== Table::Row Implementation ========================

class Table::Row::Impl
{
public:
    const Table * parent;
    std::vector<Object> objects;

    /** Default empty constructor */
    Impl() = default;

    /** Constructor of RowImpl. Receives the parent Table pointer.
     */
    Impl(const Table * parent): parent(parent) {}

}; // class Table::Row::Impl


class Table::Impl
{
public:
    ColumnVector columns;
    std::map<size_t, size_t> colIntMap;
    std::map<std::string, size_t> colStrMap;
    size_t maxColId = Column::NO_ID; // Keep track of the biggest ID
    std::vector<Table::Row> rows;

    inline size_t getIndex(size_t colId) const
    {
        return colIntMap.find(colId) != colIntMap.end() ?
               colIntMap.at(colId) : Column::NO_INDEX;
    }

    inline size_t getIndex(const std::string &colName) const
    {
        return colStrMap.find(colName) != colStrMap.end() ?
               colStrMap.at(colName) : Column::NO_INDEX;
    }

    inline const Column& getColumnByIndex(size_t index) const
    {
        return columns[index];
    }

    inline size_t addColumn(const Column &col)
    {
        insertColumn(col, columns.size());
    }

    size_t insertColumn(const Column &col, size_t pos)
    {
        size_t size = columns.size();

        if (pos > size)
            pos = size;

        auto colId = col.getId();
        auto colName = col.getName();

        //FIXME: Check if the col has ID, but it overlaps with an existing one
        if (colId == Column::NO_ID) // Find a new ID
            colId = ++maxColId; // increment maxColId and assign it to colId
        else
            maxColId = std::max(maxColId, colId);

        auto it = columns.begin() + pos;
        columns.emplace(it, colId, colName, col.getType(), col.getDescription());

        updateIndexes(columns.begin() + pos + 1, 1);

        return colStrMap[colName] = colIntMap[colId] = pos;
    } // function insertColumn

    void removeColumn(size_t index)
    {
        auto it = columns.erase(columns.begin() + index);
        updateIndexes(it, -1);
        // FIXME: Maybe if we store elements grouped by columns we can avoid
        // the following
        for (auto &row: rows)
            row.impl->objects.erase(row.impl->objects.begin() + index);
    }

    /**
     * Update the indexes of some columns when new columns are added or
     * existing one are deleted.
     * @param firstToUpdate This is the position (iterator) of the first
     * element to update
     * @param updateFactor This will be 1 if a new column is inserted
     * and -1 if a column was deleted.
     */
    void updateIndexes(ColumnVector::iterator firstToUpdate,
                       int updateFactor)
    {
        for (auto it=firstToUpdate; it < columns.end(); ++it)
        {
            auto &col = *it;
            colStrMap[col.getName()] += updateFactor;
            colIntMap[col.getId()] += updateFactor;
        }
    } // function updateIndexes
}; // class Table::Impl



Table::Row::Row(Impl *rowImpl): impl(rowImpl) {}

Table::Row::Row(const Row &other)
{
    impl = new Impl();
    *impl = *(other.impl);
} // Copy ctor

Table::Row::Row(Row &&other) noexcept
{
    std::swap(impl, other.impl);
} // Move ctor

Table::Row& Table::Row::operator=(const Row &other)
{
    *impl = *(other.impl);
    return *this;
} // Copy Ctor Table::Row

Table::Row& Table::Row::operator=(Row &&other) noexcept
{
    std::swap(impl, other.impl);
    return *this;
} // Copy Ctor Table::Row

Table::Row::~Row() { delete impl; }

const Object& Table::Row::operator[](size_t colId) const
{
    size_t index = impl->parent->impl->getIndex(colId);
    return impl->objects[index];
} // function Table::Row.operator[size_t] const

Object& Table::Row::operator[](size_t colId)
{
    size_t index = impl->parent->impl->getIndex(colId);
    return impl->objects[index];
} // function Table::Row.operator[size_t]

const Object& Table::Row::operator[](const std::string &colName) const
{
    size_t index = impl->parent->impl->getIndex(colName);
    return impl->objects[index];
} // function Table::Row.operator[string] const

Object& Table::Row::operator[](const std::string &colName)
{
    size_t index = impl->parent->impl->getIndex(colName);
    return impl->objects[index];
} // function Table::Row.operator[string]

void Table::Row::toStream(std::ostream &ostream) const
{
    size_t i = 0;
    for (auto &col: impl->parent->impl->columns)
    {
        ostream << col.getName() << ": ";
        impl->objects[i++].toStream(ostream);
        ostream << "\t";
    }
} // function Table::Row.toStream

std::ostream& operator<< (std::ostream &ostream, const Table::Row &row)
{
    row.toStream(ostream);
    return ostream;
} // operator << (Table::Row)

Table::Row::iterator Table::Row::begin()
{
    return impl->objects.begin();
}

Table::Row::iterator Table::Row::end()
{
    return impl->objects.end();
}

// ========================== Table Implementation ========================

Table::Table(std::initializer_list<Column> columns)
{
    impl = new Impl();

    for (auto& col: columns)
        impl->addColumn(col);
} // Ctor Table

Table::Table()
{
    impl = new Impl();
} // Empty Table ctor

Table::~Table()
{
    delete impl;
} // Dtor ~Table

void Table::clear()
{
    delete impl;
    impl = new Impl();
} // function Table.clear


// ---------------- Row related methods ------------------------

size_t Table::getSize() const
{
    return impl->rows.size();
} // function Table.getSize

bool Table::isEmpty() const
{
    return impl->rows.empty();
} // function Table.isEmpty

size_t Table::getIndex(size_t colId)
{
    return impl->getIndex(colId);
} // function Table.getIndex(size_t)

size_t Table::getIndex(const std::string &colName)
{
    return impl->getIndex(colName);
} // function Table.getIndex(string)

const Table::Column & Table::getColumn(size_t colId)
{
    return impl->getColumnByIndex(impl->getIndex(colId));
} // function Table.getColumnByIndex

const Table::Column& Table::getColumn(const std::string &colName)
{
    return impl->getColumnByIndex(impl->getIndex(colName));
}

const Column& Table::getColumnByIndex(size_t index)
{
    return impl->getColumnByIndex(index);
}

Table::Row Table::createRow() const
{
    auto rowImpl = new Row::Impl(this);
    for (auto &col: impl->columns)
        rowImpl->objects.emplace_back(col.getType());

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
} // function Table.begin

Table::iterator Table::end()
{
    return impl->rows.end();
} // function Table.end


const Table::Row& Table::operator[](const size_t pos) const
{
    return impl->rows[pos];
} // function Table::operator[] const

Table::Row& Table::operator[](const size_t pos)
{
    return impl->rows[pos];
} // function Table::operator[]


// ---------------- Column related methods ------------------------

size_t Table::addColumn(const Column &col)
{
    ASSERT_ERROR(!isEmpty(),
                 "A default value should be provided when add a column "
                 "to a non-empty table.");

    return impl->addColumn(col);
} // function Table.addColumn

size_t Table::addColumn(const Column &col, const Object &defaultValue)
{
    ASSERT_ERROR(col.getType() != defaultValue.getType(),
                 "The default value has not the same type of the column.");

    size_t index = impl->addColumn(col);
    for (auto &row: impl->rows)
        row.impl->objects.emplace_back(defaultValue);
    return index;
} // function Table.addColumn

size_t Table::insertColumn(const Column &col, size_t pos)
{
    ASSERT_ERROR(!isEmpty(),
                 "A default value should be provided when add a column "
                         "to a non-empty table.");
    return impl->insertColumn(col, pos);
}

size_t Table::insertColumn(const Column &col, size_t pos,
                         const Object &defaultValue)
{
    ASSERT_ERROR(col.getType() != defaultValue.getType(),
                 "The default value has not the same type of the column.");

    size_t index = impl->insertColumn(col, pos);
    for (auto &row: impl->rows)
        row.impl->objects.emplace(row.impl->objects.begin()+pos, defaultValue);

    return index;
} // function Table.addColumn

void Table::removeColumn(size_t colId)
{
    impl->removeColumn(impl->getIndex(colId));
}

void Table::removeColumn(const std::string &colName)
{

    impl->removeColumn(impl->getIndex(colName));
}

void Table::moveColumn(size_t colId, size_t pos)
{
    THROW_ERROR("Not implemented.");
}

void Table::moveColumn(const std::string &colName, size_t pos)
{
    THROW_ERROR("Not implemented.");
}

Table::const_col_iterator Table::cbegin() const
{
    return impl->columns.begin();
}

Table::const_col_iterator Table::cend() const
{
    return impl->columns.end();
}

// ========================== TableIO Implementation ===========================

using TableIOImplRegistry = ImplRegistry<TableIO::Impl>;

TableIOImplRegistry * getTableIORegistry()
{
    static TableIOImplRegistry registry;
    return &registry;
} // function getTableIORegistry

bool em::TableIO::registerImpl(const StringVector &extOrNames,
                               TableIO::ImplBuilder builder)
{
    return getTableIORegistry()->registerImpl(extOrNames, builder);
} // function registerImageIOImpl

bool TableIO::hasImpl(const std::string &extension)
{
    return getTableIORegistry()->hasImpl(extension);

} // function hasIO

TableIO::TableIO()
{
    impl = nullptr;
} // Default Ctor

TableIO::TableIO(const std::string &extOrName): TableIO()
{
    impl = getTableIORegistry()->buildImpl(extOrName);
} // ctor TableIO(std::string)

TableIO::~TableIO()
{
    delete impl;
}

void TableIO::open(const std::string &path)
{
    delete impl;  // Does it make sense to reuse impl?
    impl = getTableIORegistry()->buildImpl(Path::getExtension(path));
    impl->open(path);
} // function TableIO.open

void TableIO::close()
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    impl->close();
} // function TableIO.close

void TableIO::read(const std::string &tableName, Table &table)
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    impl->read(tableName, table);
} // TableIO.read

#include "formats/metadata_star.cpp"