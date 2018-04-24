//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <iomanip>
#include "gtest/gtest.h"

#include "em/base/metadata.h"


using namespace em;
using Column = ColumnMap::Column;


TEST(ColumnIndex, Basic)
{
    std::string c1Name = "firstCol";
    Column c1(c1Name, typeFloat);
    ASSERT_EQ(c1.getId(), ColumnMap::NO_ID);
    ASSERT_EQ(c1.getName(), c1Name);
    ASSERT_EQ(c1.getType(), typeFloat);

    std::string c2Name = "secondCol";
    Column c2(c2Name, typeInt16);
    ASSERT_EQ(c2.getId(), ColumnMap::NO_ID);
    ASSERT_EQ(c2.getName(), c2Name);
    ASSERT_EQ(c2.getType(), typeInt16);

    ColumnMap colMap;
    ASSERT_EQ(0, colMap.addColumn(c1));
    ASSERT_EQ(1, colMap.addColumn(c2));

    // Get columns stored and do some validations
    auto& rc1 = colMap[0];
    auto& rc2 = colMap[1];
    ASSERT_EQ(rc1.getId(), 1);
    ASSERT_EQ(rc2.getId(), 2);

    ASSERT_EQ(0, colMap.getIndex(c1.getName()));
    ASSERT_EQ(1, colMap.getIndex(c2.getName()));

    ASSERT_EQ(0, colMap.getIndex(rc1.getId()));
    ASSERT_EQ(1, colMap.getIndex(rc2.getId()));

    ASSERT_EQ(ColumnMap::NO_INDEX, colMap.getIndex(100));
    ASSERT_EQ(ColumnMap::NO_INDEX, colMap.getIndex("noColumn"));

    // Add more columns with and without IDs
    size_t bigId = 100;
    size_t c3index = colMap.addColumn(Column(bigId, "thirdCol", typeFloat));
    auto& rc3 = colMap[c3index];
    ASSERT_EQ(bigId, rc3.getId());
    ASSERT_EQ(std::string("thirdCol"), rc3.getName());

    colMap.addColumn(Column("forthCol", typeFloat));
    auto& rc4 = colMap[c3index + 1];
    ASSERT_EQ(bigId + 1, rc4.getId());
    ASSERT_EQ(std::string("forthCol"), rc4.getName());
} // TEST Column.Basic

void printTable(Table &table)
{
    std::cerr << std::endl <<
              "============== Table ===============" << std::endl;
    size_t w = 12;

    for (auto &col: table.getColumnMap())
        std::cerr << std::setw(2*w) << std::right << col.getName() << std::endl;

    auto &firstRow = table[0];

    std::vector<int> widths;
    for (auto &obj: firstRow)
        widths.push_back(obj.toString().size() + 2);

    int i = 0;

    for (auto &row: table)
    {
        i = 0;
        for (auto &obj: row)
            std::cerr << std::setw(widths[i++]) << std::right << obj << " ";
        std::cerr << std::endl;
    }
}

TEST(Table, Basic)
{
    Table table({Column(1, "col1", typeFloat),
                 Column(2, "col2", typeInt16),
                 Column(3, "col3", typeString)
                 });

    auto row = table.createRow();

    row[1] = 3.1416f;
    std::cerr << "Row (before set) >>> " << row << std::endl;

    row[2] = 300;
    row[3] = std::string("My name");
    std::cerr << "Row (after set) >>> " << row << std::endl;

    ASSERT_EQ(row[1], row["col1"]);
    ASSERT_EQ(row[2], row["col2"]);
    ASSERT_EQ(row[3], row["col3"]);

    int x = row[2];
    ASSERT_EQ(x, 300);

    //auto row2 = table.createRow();

    Table::Row row3(row);
    row3["col2"] = 400;
    row3["col3"] = std::string("Other name");
    x = row3[2];
    ASSERT_EQ(x, 400);

    std::cerr << "Row 3 >>>  " << row3 << std::endl;
    //row2 = row;

    table.addRow(row);
    table.addRow(row3);

    printTable(table);

    for (auto& row: table)
    {
        row["col3"] = std::string("Other name 2");
        row["col2"] = (int)row["col2"] / 10;
    }

    printTable(table);

    ASSERT_EQ(table.getSize(), 2);
    ASSERT_FALSE(table.isEmpty());

    table.clear();
    ASSERT_EQ(table.getSize(), 0);
    ASSERT_TRUE(table.isEmpty());


} // TEST Row.Basic

TEST(Table, Read)
{
    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        std::string root(testDataPath);
        root += "relion_tutorial/import/";

        std::string fn1 = root + "case1/classify3d_small_it038_data.star";
        std::cout << "Reading star: " << fn1 << std::endl;

        Table t({Column(1, "col1", typeFloat),
                 Column(2, "col2", typeInt16),
                 Column(3, "col3", typeString)
                });
        TableIO tio;
        tio.open(fn1);
        tio.read("images", t);

        printTable(t);
    }
/*
 * relion_tutorial/import/case1/classify3d_small_it038_data.star
relion_tutorial/import/case1/classify3d_small_it038_model.star
relion_tutorial/import/case1/classify3d_small_it038_optimiser.star
relion_tutorial/import/case1/classify3d_small_it038_sampling.star
relion_tutorial/import/case2/relion_it015_data.star
relion_tutorial/import/case2/relion_it015_model.star
relion_tutorial/import/case2/relion_it015_optimiser.star
relion_tutorial/import/case2/relion_it015_sampling.star
relion_tutorial/import/classify2d/extra/relion_it015_data.star
relion_tutorial/import/classify2d/extra/relion_it015_model.star
relion_tutorial/import/classify2d/extra/relion_it015_optimiser.star
relion_tutorial/import/classify2d/extra/relion_it015_sampling.star
 */
} // TEST Table.Read