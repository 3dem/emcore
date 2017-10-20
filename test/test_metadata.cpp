//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/base/metadata.h"


using namespace em;


TEST(ColumnIndex, Basic)
{
    std::string c1Name = "firstCol";
    Column c1(1, c1Name, TypeFloat);
    ASSERT_EQ(c1.getId(), 1);
    ASSERT_EQ(c1.getName(), c1Name);
    ASSERT_EQ(c1.getType(), TypeFloat);

    std::string c2Name = "secondCol";
    Column c2(2, c2Name, TypeInt16);
    ASSERT_EQ(c2.getId(), 2);
    ASSERT_EQ(c2.getName(), c2Name);
    ASSERT_EQ(c2.getType(), TypeInt16);

    ColumnIndex colIndex;
    ASSERT_EQ(0, colIndex.addColumn(c1));
    ASSERT_EQ(1, colIndex.addColumn(c2));

    ASSERT_EQ(0, colIndex[c1.getName()]);
    ASSERT_EQ(1, colIndex[c2.getName()]);

    ASSERT_EQ(0, colIndex[c1.getId()]);
    ASSERT_EQ(1, colIndex[c2.getId()]);

    ASSERT_EQ(ColumnIndex::NO_INDEX, colIndex[100]);
    ASSERT_EQ(ColumnIndex::NO_INDEX, colIndex["noColumn"]);

} // TEST Column.Basic


TEST(Row, Basic)
{
    Table table({Column(1, "col1", TypeFloat),
             Column(2, "col2", TypeInt16),
             Column(3, "col3", Type::get<std::string>())
            });

    auto row = table.createRow();

    row[1] = 3.1416f;
    std::cerr << row << std::endl;

    row[2] = 300;
    row[3] = std::string("My name");
    std::cerr << "Row: " << std::endl << row << std::endl;

    ASSERT_EQ(row[1], row["col1"]);
    ASSERT_EQ(row[2], row["col2"]);
    ASSERT_EQ(row[3], row["col3"]);

    int x = row[2];
    ASSERT_EQ(x, 300);

    row["col2"] = 400;
    x = row[2];
    ASSERT_EQ(x, 400);

    //auto row2 = table.createRow();

    Table::Row row3(row);
    std::cerr << "Row3: " << std::endl << row << std::endl;
    //row2 = row;
    //table.addRow();

} // TEST Row.Basic