//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <iostream>
#include <fstream> // the good one
#include <sstream>
#include <algorithm> // sorting the table

#include "em/base/registry.h"
#include "em/base/table.h"
#include "em/base/table_priv.h"


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

void Table::Column::toStream(std::ostream &ostream) const
{
    ostream << "<column "
            << "id=\"" << id << "\" name=\"" << name << "\" "
            << "type=\"" << type.getName() << "\" />";
} // function Table::Column.toStream

std::string Table::Column::toString() const
{
    std::stringstream ss;
    toStream(ss);
    return ss.str();
} // function Table::Column.toString


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
        auto it = colIntMap.find(colId);
        ASSERT_ERROR(it == colIntMap.end(),
                     std::string("Invalid column id: ") + std::to_string(colId));
        return it->second;
    }

    inline size_t getIndex(const std::string &colName) const
    {
        auto it = colStrMap.find(colName);
        ASSERT_ERROR(it == colStrMap.end(),
                     std::string("Invalid column name: ") + colName);
        return it->second;
    }

    inline const Column& getColumnByIndex(size_t index) const
    {
        return columns[index];
    }

    inline size_t addColumn(const Column &col)
    {
        return insertColumn(col, columns.size());
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
    ostream << "<row ";
    for (auto &obj: *const_cast<Row*>(this))
    {
        obj.toStream(ostream);
        ostream << "\t";
    }
    ostream << " />";
} // function Table::Row.toStream

std::string Table::Row::toString() const
{
    std::stringstream ss;
    toStream(ss);
    return ss.str();
} // function Table::Row.toString

Table::Row::iterator Table::Row::begin()
{
    return impl->objects.begin();
} // function Table::Row.begin

Table::Row::iterator Table::Row::end()
{
    return impl->objects.end();
} // function Table::Row.end

Table::Row::const_iterator Table::Row::cbegin() const
{
    return impl->objects.cbegin();
} // function Table::Row.begin

Table::Row::const_iterator Table::Row::cend() const
{
    return impl->objects.cend();
} // function Table::Row.end


// ========================== Table Implementation ========================

Table::Table(const std::vector<Column> &columns): Table()
{
    for (auto& col: columns)
        impl->addColumn(col);
} // Ctor Table

Table::Table()
{
    impl = new Impl();
} // Empty Table ctor

Table::Table(const Table &other): Table()
{
    *impl = *other.impl;
} // Table Copy-ctor

Table::Table(Table &&other) noexcept : Table()
{
    std::swap(impl, other.impl);
} // Table Move-ctor

Table& Table::operator=(const Table &other)
{
    *impl = *other.impl;
    return *this;
} // Table assign operator

Table& Table::operator=(Table &&other) noexcept
{
    std::swap(impl, other.impl);
    return *this;
} // Table Move-assign operator

Table::~Table()
{
    delete impl;
} // Dtor ~Table

void Table::clear()
{
    delete impl;
    impl = new Impl();
} // function Table.clear

/** Simple struct to parse column name and order (ASC or DESC) */
struct ColumnOrder
{
    std::string columnName;
    bool ascending;

    ColumnOrder(const std::string &columnString)
    {
        auto parts = String::split(columnString);
        auto n = parts.size();

        ASSERT_ERROR(n > 2, "More than two arguments as part of column sorting");

        columnName = parts[0];
        ascending = true;

        if (n == 2)
        {
            auto order = String::toUpper(parts[1]);
            if (order == "DESC")
                ascending = false;
            else
                ASSERT_ERROR(order != "ASC",
                             "Either ASC or DESC are accepted as column sorting.");
        }
    }
};

void Table::sort(const StringVector &columnName)
{
    auto co = ColumnOrder(columnName[0]);

    std::sort(impl->rows.begin(), impl->rows.end(),
              [&co](const Row &lhs, const Row &rhs)
              {
                  auto& colName = co.columnName;
                  return co.ascending ? lhs[colName] < rhs[colName] :
                                        lhs[colName] > rhs[colName];
              });
} // function Table.sort

void Table::read(const std::string &tableName, const std::string &path)
{
    auto tio = TableIO();
    tio.open(path);
    tio.read(tableName, *this);
    tio.close();
} // function Table.read

void Table::read(const std::string &path)
{
    auto tio = TableIO();
    // FIXME: Implement a way to read first table by default in TableIO
    // FIXME: it can be more efficient since the whole file will not be parsed
    tio.open(path);
    auto tableName = tio.getTableNames()[0];
    tio.read(tableName, *this);
    tio.close();
} // function Table.read


// ---------------- Row related methods ------------------------

size_t Table::getSize() const
{
    return impl->rows.size();
} // function Table.getSize

bool Table::isEmpty() const
{
    return impl->rows.empty();
} // function Table.isEmpty


std::ostream& operator<< (std::ostream &ostream, const Table::Column &col)
{
    col.toStream(ostream);
    return ostream;
} // operator << (Table::Column)

std::ostream& operator<< (std::ostream &ostream, const Table::Row &row)
{
    row.toStream(ostream);
    return ostream;
} // operator << (Table::Row)

std::ostream& operator<< (std::ostream &ostream, const Table &table)
{
    table.toStream(ostream);
    return ostream;
} // operator << (Table::Row)

void Table::toStream(std::ostream &ostream) const
{
    ostream << "<table name=\"\">" << std::endl
            << "   <columns>" << std::endl;
    for (auto &col: impl->columns)
    {
        ostream << "      ";
        col.toStream(ostream);
        ostream << std::endl;
    }
    ostream << "   </columns>" << std::endl
            << "   <rows>" << std::endl;
    for (auto &row: *(const_cast<Table*>(this)))
    {
        ostream << "      ";
        for (auto &obj: row)
        {
            obj.toStream(ostream);
            ostream << " ";
        }
        //row.toStream(ostream);
        ostream << std::endl;
    }
    ostream << "   </rows>" << std::endl
            << "</table>" << std::endl;
} // function Table.toStream

std::string Table::toString() const
{
    std::stringstream ss;
    toStream(ss);
    return ss.str();
} // function Table.toString

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
} // function Table.getColumn

const Column& Table::getColumnByIndex(size_t index)
{
    return impl->getColumnByIndex(index);
} // function Table.getColumnByIndex

size_t Table::getColumnsSize() const
{
    return impl->columns.size();
} // function Table.getColumnsSize

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

Table::const_iterator Table::cbegin() const
{
    return impl->rows.cbegin();
} // function Table.cbegin

Table::const_iterator Table::cend() const
{
    return impl->rows.cend();
} // function Table.cend

const Table::Row& Table::operator[](const size_t pos) const
{
    ASSERT_ERROR(pos >= impl->rows.size(),
                 "Requested position is greater than Table size.")
    return impl->rows[pos];
} // function Table::operator[] const

Table::Row& Table::operator[](const size_t pos)
{
    ASSERT_ERROR(pos >= impl->rows.size(),
                 "Requested position is greater than Table size.")
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

size_t Table::addColumn(const std::string &colName, const Type &colType)
{
    ASSERT_ERROR(!isEmpty(),
                 "A default value should be provided when add a column "
                 "to a non-empty table.");

    return impl->addColumn(Column(colName, colType));
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

Table::const_col_iterator Table::cbegin_cols() const
{
    return impl->columns.cbegin();
}

Table::const_col_iterator Table::cend_cols() const
{
    return impl->columns.cend();
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

void TableIO::open(const std::string &path, File::Mode mode)
{

    delete impl;  // Does it make sense to reuse impl?
    impl = getTableIORegistry()->buildImpl(Path::getExtension(path));
    impl->path = path;
    impl->fileMode = mode;

    // If the file does not exists and mode is  File::Mode::READ_WRITE
    // switch automatically to TRUNCATE mode
    auto newFile = !Path::exists(path);

    if (mode ==  File::Mode::READ_WRITE and newFile)
        impl->fileMode = File::Mode::TRUNCATE;

    ASSERT_ERROR(newFile && impl->fileMode != File::Mode::TRUNCATE,
                 String::join({"Filename does not exists: ", path}));

    impl->openFile();
} // function TableIO.open

StringVector TableIO::getTableNames() const
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    return impl->getTableNames();
} // function TableIO.getColumnNames

void TableIO::close()
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    impl->closeFile();
} // function TableIO.close

void TableIO::read(const std::string &tableName, Table &table)
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    table.clear();
    impl->read(tableName, table);
} // TableIO.read

void TableIO::write(const std::string &tableName, const Table &table)
{
    ASSERT_ERROR(impl == nullptr, "Invalid operation, implementation is null.");
    impl->write(tableName, table);
} // TableIO.write


void TableIO::Impl::openFile()
{
    file = fopen(path.c_str(), File::modeToString(fileMode));
} // function TableIO::Impl.openFile

void TableIO::Impl::closeFile()
{
    fclose(file);
} // function TableIO::Impl.closeFile

#include "table_formats/table_star.cpp"
#include "table_formats/table_sqlite.cpp"