//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <iostream>
#include <fstream> // the good one
#include <sstream>
#include <em/base/string.h>

#include "em/base/metadata.h"

using namespace em;


// ========================== Column Implementation ============================

const size_t ColumnMap::NO_ID = 0;
const size_t ColumnMap::NO_INDEX = -1;

ColumnMap::Column::Column(size_t id, const std::string &name, const Type & type,
                   const std::string &description):
                   id(id), name(name), type(type), descr(description) {}

ColumnMap::Column::Column(const std::string &name, const Type & type,
                          const std::string &description):
                    Column(NO_ID, name, type, description) {}

size_t ColumnMap::Column::getId() const { return id; }

std::string ColumnMap::Column::getName() const { return name; }

const Type & ColumnMap::Column::getType() const { return type; }

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

ColumnMap::const_iterator ColumnMap::begin() const
{
    return impl->columns.begin();
}

ColumnMap::const_iterator ColumnMap::end() const
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

const ColumnMap& Table::getColumnMap() const
{
    return impl->colIndex;
} // function Table.getColumnMap

Table::iterator Table::begin()
{
    return impl->rows.begin();
}

Table::iterator Table::end()
{
    return impl->rows.end();
}

// ========================== TableIO Implementation ===========================

class TableIO::Impl
{
public:
    FILE* file = nullptr; // Keep a file handler
    std::string path;
    std::string line; // Used for parsing the star file lines

    virtual void open(const std::string &path);
    virtual void close();
    virtual void read(const std::string &tableName, Table &table);

protected:
    void readTable(std::ifstream &ifs, Table &table);
    void readLoopColumns(std::ifstream &ifs, ColumnMap &colMap);
    void readColumns(std::ifstream &ifs, ColumnMap &colMap);
    void parseLine(const std::string &line, Table &table);
}; // class TableIO::Impl

void TableIO::Impl::open(const std::string &path)
{
    this->path = path;
    file = fopen(path.c_str(), "r");
} // TableIO::Impl.open

void TableIO::Impl::close()
{
    fclose(file);
} // TableIO::Impl.close

void TableIO::Impl::read(const std::string &tableName, Table &table)
{
    std::ifstream ifs(path.data(), std::ios_base::in);
    ifs.seekg(0);
    size_t lineCount = 0;

    while (getline(ifs, line))
    {
        ++lineCount;

        if (line.find("data_") == 0)
        {
            if (tableName.empty() || line.substr(5) == tableName)
            {
                std::cout << "Table: " << tableName << " found at line: "
                          << lineCount << std::endl;
                return readTable(ifs, table);
            }
        }

    }
} // TableIO::Impl.read

void TableIO::Impl::readTable(std::ifstream &ifs, Table &table)
{
    ColumnMap colMap;

    // Read all lines until EOF or first non-empty line
    while (getline(ifs, line) && line.empty());

    std::cout << "Before loop: '" << line << "'" << std::endl;

    if (line.find("loop_") == 0) // starts with 'loop_'
    {
        // Parse Loop column Names (all lines starting with '_')
        StringVector colNames;
        while (getline(ifs, line) && line[0] == '_')
            colNames.push_back(line.substr(1, line.find_first_of(String::SPACES)));

        line = String::trim(line);
        ASSERT_ERROR(line.empty(),
                     "There are empty lines after columns and before data");

        // TODO: Infer the Column types from the first data line
        for (auto& col: colNames)
            colMap.addColumn(ColumnMap::Column(col, typeString));

        bool moreColumns = true;

        while (!line.empty() && moreColumns)
        {
            //parseLine()
            moreColumns = bool(getline(ifs, line)); // FIXME
            line = String::trim(line);
        }

        std::cout << "Columns: " << std::endl;
        for (auto& col: colNames)
            std::cout << col << std::endl;
    }
    else
    {
        //readColumns(ifs, line, colMap, true);
    }
} // TableIO::Impl.read

void TableIO::Impl::readLoopColumns(std::ifstream &ifs, ColumnMap &colMap)
{
    std::string colName;


} // TableIO::Impl.readColumns

void TableIO::Impl::parseLine(const std::string &line, Table &table)
{
    auto row = table.createRow();
    auto& colMap = table.getColumnMap();

    std::stringstream ss(line);

    // FIXME: Change the following with a way to iterate over a Row
    // <ColumnName, Object> pairs
    for (auto& col: colMap)
        row[col.getName()].fromStream(ss);
} // function TableIO::Impl.parseLine

TableIO::TableIO()
{
    impl = new Impl();
} // Default Ctor

TableIO::TableIO(const std::string &extOrName): TableIO() {}

TableIO::~TableIO() { delete impl; }

bool TableIO::hasImpl(const std::string &extension)
{
    THROW_ERROR("NOT IMPLEMENTED. ");
} // function hasImpl

void TableIO::open(const std::string &path)
{
    impl->open(path);
} // function TableIO.open

void TableIO::close()
{
    impl->close();
} // function TableIO.close

void TableIO::read(const std::string &tableName, Table &table)
{
    impl->read(tableName, table);
} // TableIO.read

