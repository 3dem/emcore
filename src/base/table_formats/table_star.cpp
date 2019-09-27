//
// Created by josem on 4/24/18.
//

#include "em/base/table_priv.h"


using namespace emcore;


class TableIOStar: public TableIO::Impl
{
protected:
    std::string line; // Used for parsing the star file lines

    virtual void read(const std::string &tableName, Table &table) override
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
            } // if
        } // while
    } // function read

    void readTable(std::ifstream &ifs, Table &table)
    {
        // Read all lines until EOF or first non-empty line
        while (getline(ifs, line) && line.empty());

        if (line.find("loop_") == 0) // starts with 'loop_'
        {
            // Parse Loop column Names (all lines starting with '_')
            StringVector colNames;
            while (getline(ifs, line))
            {
                //FIXME: Check how to optimize this and avoid the trimming
                line = String::trim(line);
                if (line[0] != '_')
                    break;
                colNames.push_back(
                        line.substr(1, line.find_first_of(String::SPACES) - 1));
            }

            ASSERT_ERROR(line.empty(),
                         "There are empty lines after columns and before data");

            StringVector tokens = String::split(line.c_str());
            int i = 0;

            for (auto& col: colNames)
                table.addColumn(Table::Column(col, Type::inferFromString(tokens[i++])));

            auto row = table.createRow();
            bool moreRows = true;

            while (!line.empty() && moreRows)
            {
                std::stringstream ss(line);
                for (auto& obj: row)
                    obj.fromStream(ss);
                table.addRow(row);
                moreRows = bool(getline(ifs, line)); // FIXME
                line = String::trim(line);
            }
        }
        else
        {
            // TODO: pARSE key=value star file
            //readColumns(ifs, line, colMap, true);
            // Parse Loop column Names (all lines starting with '_')
            StringVector values;
            while (getline(ifs, line))
            {
                //FIXME: Check how to optimize this and avoid the trimming
                line = String::trim(line);
                if (line[0] != '_')
                    break;
                auto spacePos = line.find_first_of(String::SPACES);
                values.emplace_back(String::trim(line.substr(spacePos)));
                table.addColumn(Table::Column(line.substr(1, spacePos - 1),
                                              Type::inferFromString(values.back())));
            }
            auto row = table.createRow();
            size_t i = 0;
            for (auto& obj: row)
                obj.fromString(values[i++]);
            table.addRow(row);
        }
    } // function readTable

    virtual void write(const std::string &tableName, const Table &table) override
    {
        fprintf(file, "\ndata_%s\n\n", tableName.c_str());

        if (table.getSize() == 1)
        {
            // Compute the largest column name width for aesthetics
            size_t maxwidth = 0;
            for (auto it = table.cbegin_cols(); it < table.cend_cols(); ++it)
                maxwidth = std::max(maxwidth, it->getName().size());

            auto &row = table[0];
            auto format = String::join({"_%-", std::to_string(maxwidth), "s %s\n"});

            for (auto it = table.cbegin_cols(); it < table.cend_cols(); ++it)
                fprintf(file, format.c_str(), it->getName().c_str(),
                        row[it->getName()].toString().c_str());
        }
        else
        {
            fprintf(file, "loop_\n");
            Object counter = Object(typeInt16);
            int c = 0;

            for (auto it = table.cbegin_cols();
                 it < table.cend_cols(); ++it, ++c)
                fprintf(file, "_%s #%d\n", it->getName().c_str(), c);

            for (auto rowIt = table.cbegin(); rowIt < table.cend(); ++rowIt)
            {
                for (auto objIt = rowIt->cbegin();
                     objIt < rowIt->cend(); ++objIt)
                    fprintf(file, "%s ", objIt->toString().c_str());
                fprintf(file, "\n");
            }
        } // else n > 1
        fprintf(file, "\n");
    } // function write

    virtual StringVector getTableNames() const override
    {
        std::string line;

        std::ifstream ifs(path.data(), std::ios_base::in);
        ifs.seekg(0);
        StringVector names;

        while (getline(ifs, line))
            if (line.find("data_") == 0)
                names.emplace_back(String::trim(line.substr(5)));
        return names;
    } // function getTableNames

}; // class TableIOStar


REGISTER_TABLE_IO(StringVector({"star", "xmd"}), TableIOStar);