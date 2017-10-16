//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#ifndef EM_CORE_METADATA_H
#define EM_CORE_METADATA_H


#include <string>

#include "em/base/type.h"
#include "em/base/object.h"


namespace em {

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

        /**
         * Constructor of a Column.
         */
         Column(size_t id, const std::string &name, ConstTypePtr type,
                const std::string &description="");

        /** Return the ID of this Column */
        size_t getId() const;

        /** Return the NAME of this Column */
        std::string getName() const;

        /** Return the Type of this Column */
        ConstTypePtr getType() const;

        /** Return the description of this Column */
        std::string getDescription() const;

    private:
        size_t id;
        std::string name;
        ConstTypePtr type;
        std::string descr = "";

    }; // class Column

    using ColumnVector = std::vector<Column>;

    /**
     * Map between column IDs and NAMEs to a given INDEX of the column.
     * Indexes start at 0.
     */
    class ColumnIndex
    {
    public:
        static const size_t NO_INDEX;

        /** Add a new column, return the column index. */
        size_t addColumn(const Column &column);

        /** Return the index of the column specified by columnId. */
        size_t operator[](size_t columnId);

        /** Return the index of the column specified by columnName. */
        size_t operator[](const std::string &columnName);

        /** Return how many columns there are in the Index. */
        size_t size() const;

    private:
        ColumnVector columns;
        std::map<size_t, size_t> colIntMap;
        std::map<std::string, size_t> colStrMap;

    }; // class ColumnIndex


    /**
     * Class to store several rows of data values.
     * Each Row will contain value objects that are mapped to the Columns
     * defined in this Table.
     */
    class Table
    {
    private:
        /** Implementation class for Row adn Table to use the PIMPL idiom */
        class RowImpl;
        class TableImpl;
        TableImpl * impl = nullptr;

    public:
        /**
         * Class to hold key-value pairs. Keys will be either string or integer
         * representing a given column. Values are instances of class Object
         * that can hold values of different types. The Type of a given object
         * in the Row should be of the same type of the Column's Type.
         */
        class Row
        {
        public:
            /** Return the index of the column specified by columnId. */
            Object& operator[](size_t columnId);

            /** Return the index of the column specified by columnName. */
            Object& operator[](const std::string &columnName);

            /** Row Dtor */
            ~Row();
            /** Copy constructor and assignment */
            Row(const Row& other);
            Row& operator=(const Row& other);

        private:
            RowImpl * impl = nullptr;
            /** Construction of a Row, given its implementation.
             * Only accesible by Table.
             */
             Row(RowImpl * rowImpl);

            friend class Table;
        }; // class Row

        /** Constructor of Table base of input Columns */
        Table(const ColumnVector &columns);

        /** Destructor for Table */
        virtual ~Table();

        /** Create a new row with the columns defined in this Table. */
        Row createRow() const;

        /** Add a new row to the set */
        bool addRow(const Row & row);


    }; // class Table



} // namespace em

#endif //EM_CORE_METADATA_H
