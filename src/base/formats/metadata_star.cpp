//
// Created by josem on 4/24/18.
//

#include "em/base/metadata_priv.h"


using namespace em;


class TableIOStar: public TableIO::Impl
{
public:
    FILE* file = nullptr; // Keep a file handler
    std::string line; // Used for parsing the star file lines

    virtual void open(const std::string &path) override
    {
        this->path = path;
        file = fopen(path.c_str(), "r");
    } // function open

    virtual void close() override
    {
        fclose(file);
    } // function close

    virtual void read(const std::string &tableName, Table &table) override
    {
        table.clear();

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
            } // if
        } // while
    } // function read

protected:
    void readTable(std::ifstream &ifs, Table &table)
    {
        // Read all lines until EOF or first non-empty line
        while (getline(ifs, line) && line.empty());

        std::cout << "Before loop: '" << line << "'" << std::endl;

        if (line.find("loop_") == 0) // starts with 'loop_'
        {
            // Parse Loop column Names (all lines starting with '_')
            StringVector colNames;
            while (getline(ifs, line))
            {
                line = String::trim(line); //FIXME: Check how to optimize this and avoid the triming
                if (line[0] != '_')
                    break;
                colNames.push_back(
                        line.substr(1, line.find_first_of(String::SPACES) - 1));
            }


            ASSERT_ERROR(line.empty(),
                         "There are empty lines after columns and before data");

            // TODO: Infer the Column types from the first data line
            for (auto& col: colNames)
                table.addColumn(ColumnMap::Column(col, typeString));

            auto row = table.createRow();

            bool moreColumns = true;

            while (!line.empty() && moreColumns)
            {
                //parseLine()
                moreColumns = bool(getline(ifs, line)); // FIXME
                line = String::trim(line);
                parseLine(line, table, row);
                table.addRow(row);
            }
        }
        else
        {
            //readColumns(ifs, line, colMap, true);
        }
    } // function readTable

    void readLoopColumns(std::ifstream &ifs, ColumnMap &colMap);
    void readColumns(std::ifstream &ifs, ColumnMap &colMap)
    {

    } // function readColumns

    void parseLine(const std::string &line,
                   const Table &table, Table::Row &row)
    {
        auto& colMap = table.getColumnMap();

        std::stringstream ss(line);

        // FIXME: Change the following with a way to iterate over a Row
        // <ColumnName, Object> pairs
        for (auto& col: colMap)
            row[col.getName()].fromStream(ss);
    } // function parseLine

}; // class TableIOStar


REGISTER_TABLE_IO(StringVector({"star", "xmd"}), TableIOStar);