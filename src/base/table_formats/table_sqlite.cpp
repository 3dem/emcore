//
// Created by josem on 4/24/18.
//

#include <sqlite3.h>
#include "em/base/table_priv.h"


using namespace em;


using setObjectFunc = void (*)(Object &obj, sqlite3_stmt* stmt, int col);

void setObjectFromText(Object& obj, sqlite3_stmt* stmt, int col)
{
    obj.set(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col))));
}

void setObjectFromInt(Object& obj, sqlite3_stmt* stmt, int col)
{
    obj.set(sqlite3_column_int(stmt, col));
}

void setObjectFromFloat(Object& obj, sqlite3_stmt* stmt, int col)
{
    obj.set(sqlite3_column_double(stmt, col));
}

void setObjectFromNull(Object& obj, sqlite3_stmt* stmt, int col)
{
    // Do nothing for Null type values.
}

class TableIOSqlite: public TableIO::Impl
{
private:

    sqlite3* db;
    std::vector<setObjectFunc> funcs;

protected:
    virtual void openFile() override
    {
        int rc = sqlite3_open(path.c_str(), &db);

        if (rc != SQLITE_OK)
           THROW_ERROR(String::join({"Cannot open database: ",
                                    sqlite3_errmsg(db)}));
    }

    virtual void closeFile() override
    {
        sqlite3_close(db);
    }

    virtual StringVector getTableNames() const override
    {
        char *err_msg = nullptr;
        sqlite3_stmt *stmt;
        auto sql = "SELECT name FROM sqlite_master WHERE type='table' "
                "AND name NOT LIKE 'sqlite_%';";
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

        if (rc != SQLITE_OK )
        {
            sqlite3_free(err_msg);
            THROW_ERROR(String::join({"Failed to select Table names.\n",
                                      "Query: ", sql, "\nError: ", err_msg}));
        }

        StringVector names;
        int step;
        while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
            names.emplace_back(reinterpret_cast<const char*>(
                                       sqlite3_column_text(stmt, 0)));

        sqlite3_finalize(stmt);

        return names;
    } // function getTableNames

    virtual void read(const std::string &tableName, Table &table) override
    {
        char *err_msg = nullptr;
        sqlite3_stmt *stmt;
        auto sql = String::join({"SELECT * FROM ", tableName});
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

        if (rc != SQLITE_OK )
        {
            sqlite3_free(err_msg);
            THROW_ERROR(String::join({"Error reading table: ", tableName,
                                      "\nQuery: ", sql, "\nError: ", err_msg}));
        }

        int nCols = sqlite3_column_count(stmt);
        funcs.clear();

        for (int i = 0; i < nCols; ++i)
            table.addColumn(sqlite3_column_name(stmt, i),
                            getTypeFromSqlite(sqlite3_column_type(stmt, i)));

        int step;
        auto row = table.createRow();

        while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            int i = 0;
            for (auto& obj: row)
            {
                funcs[i](obj, stmt, i);
                ++i;
            }
        }

        sqlite3_finalize(stmt);
    } // function read

    virtual void write(const std::string &tableName, const Table &table) override
    {
    } // function write

    /** Return the em::Type from a given SQLite type.
     * Each call to this function will also store the function that
     * will be used to retrieve the object */
    virtual em::Type getTypeFromSqlite(int sqliteType)
    {
        switch (sqliteType)
        {
            case SQLITE3_TEXT:
                funcs.push_back(setObjectFromText);
                return em::typeString;
            case SQLITE_INTEGER:
                funcs.push_back(setObjectFromInt);
                return em::typeInt32;
            case SQLITE_FLOAT:
                funcs.push_back(setObjectFromFloat);
                return em::typeFloat;
            case SQLITE_NULL:
                funcs.push_back(setObjectFromNull);
                return em::typeNull;
            default:
                THROW_ERROR(String::join({"Unsupported SQLite type",
                                          std::to_string(sqliteType)}));
        }
    } // function getTypeFromSqlite

}; // class TableIOStar


REGISTER_TABLE_IO(StringVector({"sqlite", "db"}), TableIOSqlite);