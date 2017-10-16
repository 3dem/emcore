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
    Table t({Column(1, "col1", TypeFloat),
             Column(2, "col2", TypeInt16),
             Column(3, "col3", Type::get<std::string>())
            });

    auto row = t.createRow();

    row[1] = 3.1416f;

} // TEST Row.Basic