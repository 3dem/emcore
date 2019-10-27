//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#ifndef EM_CORE_METADATA_H
#define EM_CORE_METADATA_H


#include <string>

#include "emc/base/type.h"
#include "emc/base/object.h"
#include "emc/base/string.h"
#include "emc/os/filesystem.h"


namespace emcore {

    /**
     * Class to store several rows of data values.
     * Each Row will contain value objects that are mapped to the Columns
     * defined in this Table.
     */
    class Table
    {
    public:
        /**
         * Class defining the properties of a given column in a Row or Table.
         * Each column should have an integer ID and a string NAME (both of which
         * should be unique among a given set of Columns). Additionally, a Column
         * should have a Type and could have a description of its meaning.
        */
        class Column
        {
        public:
            static const size_t NO_ID;
            static const size_t NO_INDEX;

            /** Constructor of a Column given the ID */
            Column(size_t id, const std::string &name, const Type & type,
                   const std::string &description="");
            /** Constructor of a Column without ID */
            Column(const std::string &name, const Type & type,
                   const std::string &description="");

            /** Return the ID of this Column */
            size_t getId() const;

            /** Return the NAME of this Column */
            std::string getName() const;

            /** Return the Type of this Column */
            const Type & getType() const;

            /** Return the description of this Column */
            std::string getDescription() const;

            void toStream(std::ostream &ostream) const;
            std::string toString() const;

        private:
            size_t id;
            std::string name;
            Type type;
            std::string descr = "";

            friend class Table;

        }; // class Column

        using ColumnVector = std::vector<Column>;
        using col_iterator = ColumnVector::iterator;
        using const_col_iterator = ColumnVector::const_iterator;

        /**
         * Class to hold key-value pairs. Keys will be either string or integer
         * representing a given column. Values are instances of class Object
         * that can hold values of different types. The Type of a given object
         * in the Row should be of the same type of the Column's Type.
         */
        class Row
        {
        public:
            /** Empty constructor */
            Row() = default;

            /** Copy constructor and assignment */
            Row(const Row& other);
            /** Move constructor */
            Row(Row&& other) noexcept;

            /** Row Dtor */
            ~Row();

            /** Return the index of the column specified by colId. */
            const Object& operator[](size_t colId) const;
            Object& operator[](size_t colId);

            /** Return the index of the column specified by colName. */
            const Object& operator[](const std::string &colName) const;
            Object& operator[](const std::string &colName);

            Row& operator=(const Row& other);
            Row& operator=(Row&& other) noexcept;

            void toStream(std::ostream &ostream) const;
            std::string toString() const;

            // TODO: Hide implementation details and implement a proper iterator
            using iterator = std::vector<Object>::iterator;
            using const_iterator = std::vector<Object>::const_iterator;

            iterator begin();
            iterator end();
            const_iterator cbegin() const;
            const_iterator cend() const;

        private:
            class Impl;
            Impl * impl = nullptr;
            /** Construction of a Row, given its implementation.
             * Only accessible by Table.
             */
             Row(Impl * rowImpl);

            friend class Table;
        }; // class Row

        using RowVector = std::vector<Row>;
        using iterator = RowVector::iterator;
        using const_iterator = RowVector::const_iterator;

        /** Empty Table constructor */
        Table();

        /** Copy constructor */
        Table(const Table &other);

        /** Move constructor */
        Table(Table &&other) noexcept;

        /** Assign operators */
        Table& operator=(const Table &other);
        Table& operator=(Table &&other) noexcept;

        /** Table constructor based on input columns */
        Table(const std::vector<Column> &columns);

        /** Destructor for Table */
        virtual ~Table();

        /** Clear all columns and rows */
        void clear();

        /** Return the number of rows in the Table */
        size_t getSize() const;

        /** Return true if the number of rows is 0 */
        bool isEmpty() const;

        void toStream(std::ostream &ostream) const;
        std::string toString() const;

        /** Return the index of the column with this ID */
        size_t getIndex(size_t colId);

        /** Return the index of the column with this NAME. */
        size_t getIndex(const std::string &colName);

        /** Return the column with this column ID. */
        const Column& getColumn(size_t colId);

        /** Return the column with this NAME */
        const Column& getColumn(const std::string &colName);

        /** Return the column in the given INDEX */
        const Column& getColumnByIndex(size_t index);

        /** Return number of columns in the table */
        size_t getColumnsSize() const;

        /**
         * Add a column when the table does not contain any row.
         * If there are rows, the other function should be used where
         * a default value should be provided.
         * @param col Column to be added
         */
        size_t addColumn(const Column &col);
        size_t addColumn(const std::string &colName, const Type& colType);

        // TODO: Check if it is better to put the default value
        // as part of the column
        /**
         * Add a new column when the table already contains some rows.
         * */
        size_t addColumn(const Column &col, const Object &defaultValue);

        /** Insert a new column at a given position */
        size_t insertColumn(const Column &col, size_t pos);
        size_t insertColumn(const Column &col, size_t pos,
                            const Object &defaultValue);

        /** Remove an existing column from the Table */
        void removeColumn(size_t colId);
        void removeColumn(const std::string &colName);

        /** Change the position of a column */
        void moveColumn(size_t colId, size_t pos);
        void moveColumn(const std::string &colName, size_t pos);

        /** Return column iterator at the beginning */
        const_col_iterator cbegin_cols() const;

        /** Return column iterator at the end */
        const_col_iterator cend_cols() const;

        /** Return the row at this position */
        const Row& operator[](size_t pos) const;
        Row& operator[](size_t pos);

        /** Create a new row with the columns defined in this Table. */
        Row createRow() const;

        /** Add a new row to the set */
        bool addRow(const Row &row);

        /** Insert a new row in a given position */
        bool insertRow(const Row &row, size_t pos);

        /** Delete a given row */
        bool deleteRow(const Row &row);

        /** Delete all rows that match a query string
         * @returns The number of deleted rows.
         */
        size_t deleteRows(const std::string &queryStr);

        /** Update a given row */
        bool updateRow(const Row &row);

        /** Update many rows at once. Apply the operation string to all
         * rows that match the query string.
         * @returns The number of updated rows.
         */
        size_t updateRows(const std::string &operation,
                          const std::string &queryStr);

        // TODO: We can extend the columnName input string to allow
        // more columns and ASC/DESC
        /** Sort the table by a given column name.
         */
         void sort(const StringVector &columnName);

        /** Shortcut function to read a table from file without the need
         * to explicitly instantiating a TableFile object. After calling
         * this function, the current table will be populated.
         * @param tableName Name of the table that will be read
         * @param path Path to the table file, the type of reader will
         *  be inferred from the filename extension
         */
         void read(const std::string &tableName, const std::string &path);

        /**
         * Shortcut function to read a table from file without the need
         * to explicitly instantiating a TableFile object. After calling
         * this function, the current table will be populated.
         * @param path Path to the table file, the type of reader will
         *  be inferred from the filename extension
         */
        void read(const std::string &path);


        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;

    private:
        /** Implementation class for Row and Table to use the PIMPL idiom */
        class Impl;
        Impl * impl = nullptr;

    }; // class Table


    /** @ingroup image
     * Read and write metadata (as table) from/to files.
     *
     * Internally, the TableFile class holds a pointer to TableIOImpl class,
     * that contains the details about how to open files and read the metadata.
     * The TableIOImpl class should be extended to provide support for other
     * formats.
     */
    class TableFile
    {
    public:
        class Impl;

        /** Used when registering new Impl classes.
         * The ImplBuilder is a function that should return a pointer to
         * a newly created implementation.
         */
        using ImplBuilder = Impl* (*)();

        /**
         * Empty constructor for TableFile.
         * In this case the newly created instance will have no format
         * implementation associated to read/write formats. Then, when the
         * open() method is called to open a file, the format implementation
         * will be inferred from the filename extension. Some functions will
         * raise an exception if called without having opened a file and,
         * therefore, without having an underlying format implementation.
         */
        TableFile();

        /**
         * Constructor to build a new TableFile instance given its name or
         * an extension related to the format implementation. The provided
         * input string should be the key associated to a know format
         * implementation. If not, an exception will be thrown. If the format
         * implementation is associated to the TableFile instance, it will not
         * change when calling the open() method. This allow to read/write
         * metadata with unknown (or non-standard) file extensions.
         *
         * @param extOrName Input string representing either the TableFile name
         * or one of the extensions registered for it.
         */
        TableFile(const std::string &extOrName);

        ~TableFile();

        /**
         * Check if some TableFile implementation is registered for a given name
         * or extension.
         *
         * @param extOrName Input string representing either the TableFile name
         * or one of the extensions registered.
         * @return Return True if there is any TableFile registered.
         */
        static bool hasImpl(const std::string &extOrName);

        /**
         * Register a new TableFile implementation.
         * This function should not be used unless you are developing an
         * implementation for a new TableFile format.
         */
        static bool registerImpl(const StringVector &extOrNames,
                                 ImplBuilder builder);

        // TODO: DOCUMENT
        void open(const std::string &path,
                  const File::Mode mode=File::Mode::READ_ONLY);

        // TODO: DOCUMENT
        void close();

        // TODO: DOCUMENT
        StringVector getTableNames() const;

        // TODO: DOCUMENT
        void read(const std::string &tableName, Table &table);

        // TODO: DOCUMENT
        void write(const std::string &tableName, const Table &table);

        // String representation
        void toStream(std::ostream &ostream) const;

    private:
        // Pointer to implementation class, PIMPL idiom
        Impl* impl = nullptr;
    }; // class TableFile

    std::ostream& operator<< (std::ostream &ostream, const Table::Column &col);
    std::ostream& operator<< (std::ostream &ostream, const Table::Row &row);
    std::ostream& operator<< (std::ostream &ostream, const Table &table);
} // namespace emcore


#endif //EM_CORE_METADATA_H
