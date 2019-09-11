//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <iomanip>
#include <random>
#include "gtest/gtest.h"

#include "em/base/table.h"
#include "em/base/timer.h"

#include "test_common.h"


using namespace em;
using Column = Table::Column;


Table createTable(size_t nRows)
{
    Table table({Column(1, "col1", typeSizeT),
                 Column(2, "col2", typeFloat),
                 Column(3, "col3", typeString)
                });

    auto row = table.createRow();

    for (size_t i = 0; i < nRows; ++i)
    {
        row["col1"] = i;
        row["col2"] = float(i) / 100;
        row["col3"] = std::string("image_") + row["col1"].toString();
        table.addRow(row);
    }

    return table;
}

void checkColumns(Table &table, std::vector<int> indexes={})
{
    StringVector colNames = {"col1", "col2", "col3"};

    if (indexes.empty())
        indexes = {0, 1, 2};

    // Check all expected columns are there
    for (int i = 0; i < table.getColumnsSize(); ++i)
        ASSERT_EQ(colNames[indexes[i]], table.getColumnByIndex(i).getName());
}


TEST(Table, ColumnsBasic)
{
    std::string c1Name = "firstCol";
    Column c1(c1Name, typeFloat);
    ASSERT_EQ(c1.getId(), Column::NO_ID);
    ASSERT_EQ(c1.getName(), c1Name);
    ASSERT_EQ(c1.getType(), typeFloat);

    std::string c2Name = "secondCol";
    Column c2(c2Name, typeInt16);
    ASSERT_EQ(c2.getId(), Column::NO_ID);
    ASSERT_EQ(c2.getName(), c2Name);
    ASSERT_EQ(c2.getType(), typeInt16);

    Table colMap;
    ASSERT_EQ(0, colMap.addColumn(c1));
    ASSERT_EQ(1, colMap.addColumn(c2));

    // Get columns stored and do some validations
    auto& rc1 = colMap.getColumnByIndex(0);
    auto& rc2 = colMap.getColumnByIndex(1);
    ASSERT_EQ(rc1.getId(), 1);
    ASSERT_EQ(rc2.getId(), 2);

    ASSERT_EQ(0, colMap.getIndex(c1.getName()));
    ASSERT_EQ(1, colMap.getIndex(c2.getName()));

    ASSERT_EQ(0, colMap.getIndex(rc1.getId()));
    ASSERT_EQ(1, colMap.getIndex(rc2.getId()));

    ASSERT_THROW(colMap.getIndex(100), Error);
    ASSERT_THROW(colMap.getIndex("noColumn"), Error);

    // Add more columns with and without IDs
    size_t bigId = 100;
    size_t c3index = colMap.addColumn(Column(bigId, "thirdCol", typeFloat));
    auto &rc3 = colMap.getColumnByIndex(c3index);
    ASSERT_EQ(bigId, rc3.getId());
    ASSERT_EQ(std::string("thirdCol"), rc3.getName());

    colMap.addColumn(Column("forthCol", typeFloat));
    auto &rc4 = colMap.getColumnByIndex(c3index + 1);
    ASSERT_EQ(bigId + 1, rc4.getId());
    ASSERT_EQ(std::string("forthCol"), rc4.getName());

    // Let's insert a new column, all indexes before should not be
    // changed, but the ones after the position should be increased by 1
    size_t c3bindex = colMap.insertColumn(Column("thirdBCol", typeFloat),
                                          c3index + 1);
    ASSERT_EQ(c3bindex, c3index + 1);

    size_t i = 0;
    for (auto &colName: {"firstCol", "secondCol", "thirdCol"})
        ASSERT_EQ(colMap.getIndex(colName), i++);

    ASSERT_EQ(colMap.getIndex("forthCol"), c3bindex + 1);

    // Test to print some columns
    rc3.toStream(std::cout);
    rc4.toStream(std::cout);

    std::cout << "Column rc3: " << rc3 << std::endl;
    std::cout << "Column rc4: " << rc4 << std::endl;

} // TEST Column.Basic


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

    int x = row[2].get<int>();
    ASSERT_EQ(x, 300);

    //auto row2 = table.createRow();

    Table::Row row3(row);
    row3["col2"] = 400;
    row3["col3"] = std::string("Other name");
    x = row3[2].get<int>();
    ASSERT_EQ(x, 400);

    std::cerr << "Row 3 >>>  " << row3 << std::endl;
    //row2 = row;

    table.addRow(row);
    table.addRow(row3);

    for (auto& row: table)
    {
        row["col3"] = std::string("Other name 2");
        row["col2"] = row["col2"].get<int>() / 10;
    }

    //printTable(table);
    std::cout << table << std::endl;

    ASSERT_EQ(table.getSize(), 2);
    ASSERT_FALSE(table.isEmpty());

    table.clear();
    ASSERT_EQ(table.getSize(), 0);
    ASSERT_TRUE(table.isEmpty());
} // TEST Row.Basic


TEST(Table, Copy)
{
    size_t n = 10;
    auto table10 = createTable(n);
    Table table10copy(table10);
    checkColumns(table10copy);
    ASSERT_EQ(table10copy.getSize(), table10.getSize());
    ASSERT_EQ(table10copy.getSize(), n);

    for (size_t i = 0; i < n; ++i)
    {
        auto &row1 = table10[i];
        auto &row2 = table10copy[i];
        for (auto it = table10.cbegin_cols(); it < table10.cend_cols(); ++it)
        {
            auto colName = it->getName();
            ASSERT_EQ(row1[colName], row2[colName]);
        }
    }
} // Test Table.Copy


TEST(Table, RemoveColumns)
{
    // Let's create a table with no rows
    auto table0 = createTable(0);
    ASSERT_EQ(table0.getSize(), 0);
    ASSERT_TRUE(table0.isEmpty());

    // Check all expected columns are there
    checkColumns(table0);
    // Remove a column from empty table works
    table0.removeColumn("col2");
    checkColumns(table0, {0, 2});

    auto table10 = createTable(10);
    checkColumns(table10);
    table10.removeColumn("col2");
    checkColumns(table10, {0, 2});
} // TEST Row.RemoveColumns


// check the expected number of rows and some values
void checkColumnsAndRows(Table &t, int startingColIndex=0)
{
    StringVector refColNames = {"rlnVoltage", "rlnDefocusU",
                                "rlnSphericalAberration", "rlnAmplitudeContrast",
                                "rlnImageName", "rlnNormCorrection",
                                "rlnMicrographName", "rlnGroupNumber",
                                "rlnOriginX", "rlnOriginY", "rlnAngleRot",
                                "rlnAngleTilt", "rlnAnglePsi",
                                "rlnClassNumber", "rlnLogLikeliContribution",
                                "rlnNrOfSignificantSamples",
                                "rlnMaxValueProbDistribution"};

    // Check that columns are properly parsed and match the expected ones
    int i = startingColIndex;
    for (auto it = t.cbegin_cols(); it < t.cend_cols(); ++it)
    {
        auto &col = *it;
        ASSERT_EQ(refColNames[i++], col.getName());
    }

    // Check the number of rows is correct
    ASSERT_EQ(79, t.getSize());
    // Check that certain rows have the expected values
    std::vector<int> refIndexes = {0, 19, 39, 59};
    StringVector refImageName = {
            "000080@cluster/wind2/win_05677.dat",
            "000021@cluster/wind2/win_00846.dat",
            "000025@cluster/wind2/win_00165.dat",
            "000006@cluster/wind2/win_03167.dat"
    };

    StringVector  refMicrographName = {
            "cluster/wind2/win_00849.dat",
            "cluster/wind2/win_04600.dat",
            "cluster/wind2/win_03146.dat",
            "cluster/wind2/win_04772.dat"
    };

    for (i=0; i < refIndexes.size(); ++i)
    {
        auto &row = t[refIndexes[i]];
        ASSERT_EQ(refImageName[i], row["rlnImageName"].toString());
        ASSERT_EQ(refMicrographName[i], row["rlnMicrographName"].toString());
    }
}


TEST(Table, ReadStar)
{
    ASSERT_TRUE(TableIO::hasImpl("star"));

    Table table(
        {Column(1, "col1", typeFloat),
         Column(2, "col2", typeInt16),
         Column(3, "col3", typeString)
        });

    ASSERT_EQ(table.cend_cols() - table.cbegin_cols(), 3);
    ASSERT_EQ(table.getColumnsSize(), 3);
    ASSERT_TRUE(table.isEmpty());

    auto td = TestData();
    std::string root = td.get("relion_tutorial/import/");

    std::string fn1 = root + "case1/classify3d_small_it038_data.star";
    std::cout << "Reading star: " << fn1 << std::endl;

    auto table0 = Table();
    table0.read(fn1);  // Test default read of first table

    table.read("images", fn1);
    checkColumnsAndRows(table);

/*
 * relion_tutorial/import/case1/classify3d_small_it038_data.star
   xmipp_tutorial/gold/images200k.xmd
 */
} // TEST Table.Read

TEST(Table, ReadWriteStarSingleRow)
{
    auto td = TestData();
    std::string root = td.get("relion_tutorial/import/");

    std::string fn1 = root + "case1/classify3d_small_it038_optimiser.star";
    std::cout << "Reading star: " << fn1 <<
    std::endl;

    TableIO tio;
    Table t;
    tio.open(fn1);
    tio.read("optimiser_general", t);
    tio.close();
    ASSERT_EQ(t.getColumnsSize(), 52);
    ASSERT_EQ(t.getSize(), 1);

    tio.open("test-written-row.star", File::Mode::TRUNCATE);
    tio.write("my_optimiser", t);
    tio.close();
} // TEST ReadSingleRow

TEST(Table, WriteStar)
{
    auto table10 = createTable(10);
    TableIO tio;
    tio.open("test-write.star", File::Mode::TRUNCATE);
    tio.write("Particles1", table10);
    std::string str;

    for (auto &row: table10)
    {
        row["col1"] = row["col1"].get<int>() * 10;
        row["col2"] = row["col2"].get<float>() * 10 + 0.5;
        row["col3"] = String::join({row["col3"].toString(), ".mrc"});
    }
    tio.write("Particles2", table10);
    tio.close();

    auto td = TestData();
    std::string root = td.get("relion_tutorial/import/");

    std::string fn1 = root + "case1/classify3d_small_it038_data.star";
    std::cout << "Reading star: " << fn1 << std::endl;

    Table t;
    t.read("images", fn1);
    t.removeColumn("rlnVoltage");
    t.removeColumn("rlnDefocusU");
    t.removeColumn("rlnSphericalAberration");
    t.removeColumn("rlnAmplitudeContrast");

    string outFn = "images-less-cols.star";
    tio.open(outFn, File::Mode::TRUNCATE);
    tio.write("images", t);
    tio.close();

    t.read("images", outFn);
    checkColumnsAndRows(t, 4);

} // TEST Table.WriteStar


TEST(Table, ReadStarMultipleTables)
{
    auto td = TestData();
    std::string root = td.get("relion_tutorial/import/");

    std::string fn1 = root + "case1/classify3d_small_it038_sampling.star";
    std::cout << "Reading star: " << fn1 << std::endl;

    TableIO tio;
    Table t;
    tio.open(fn1);
    int c = 0;

    StringVector goldNames = {"sampling_general", "sampling_directions"};
    for (auto &name: tio.getTableNames())
    {
        std::cout << "table: " << name << std::endl;
        ASSERT_EQ(name, goldNames[c++]);
    }
    tio.close();
} // TEST(Table.ReadStarMultipleTables)


TEST(Table, ReadXmd)
{
    auto td = TestData();

    ASSERT_TRUE(TableIO::hasImpl("xmd"));

    std::string fn1 = td.get("xmipp_tutorial/gold/images200k.xmd");

    std::cout << "Reading xmd: " << fn1 << std::endl;

    Table t;
    TableIO tio;

    tio.open(fn1);

    Timer timer;
    timer.tic();
    tio.read("noname", t);
    timer.toc();
    tio.close();

    std::cout << "Size: " << t.getSize() << std::endl;

    int i = 0;
    for (auto it = t.cbegin_cols(); it < t.cend_cols(); ++it)
    {
        auto &col = *it;
        std::cout << "'" << col.getName() << "', ";
    }
} // TEST Table.ReadXmd


TEST(Table, ReadSqlite)
{
    auto td = TestData();

    std::string fn1 = td.get("xmipp_tutorial/particles/BPV_particles.sqlite");

    std::cout << "Reading sqlite: " << fn1 << std::endl;

    TableIO tio;
    Table t;
    tio.open(fn1);
    int c = 0;
    StringVector goldNames = {"Properties", "Classes", "Objects"};
    for (auto &name: tio.getTableNames())
    {
        std::cout << "table: " << name << std::endl;
        ASSERT_EQ(name, goldNames[c++]);
    }
    tio.read("Properties", t);
    std::cout << t << std::endl;

    tio.close();
} // TEST(Table, ReadSqlite)


TEST(Table, Sort)
{
    size_t N = 10000;

    auto t = createTable(N);

    // TODO: Create a simple wrapper to uniform random distribution
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1.0, 10.0);

    std::vector<double> v;

    for (int i = 0; i < N; ++i)
    {
        v.push_back(dist(mt));
        t[i]["col2"].set(v[i]);
    }

    Timer timer;
    timer.tic();

    std::sort(v.begin(), v.end());

    timer.toc("Sorted vector");

    timer.tic();
    t.sort({"col2"});
    timer.toc("Sorted table by float");

    for (int i = 0; i < N; ++i)
    {
        //std::cout << v[i] << " ";
        float f = t[i]["col2"].get<float>();
        ASSERT_FLOAT_EQ(v[i], f);
    }
    std::cout << std::endl;

    //std::cout << t << std::endl;

    timer.tic(); // Sort by string column
    t.sort({"col3"});
    timer.toc("Sorted table by string");

    // Sorting descendent
    t.sort({"col2 DESC"});
    //std::cout << t << std::endl;
    for (int i = 0; i < N; ++i)
    {
        //std::cout << v[i] << " ";
        float f = t[i]["col2"].get<float>();
        ASSERT_FLOAT_EQ(v[N-i-1], f);
    }

} // TEST Table.Sort