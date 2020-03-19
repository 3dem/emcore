
from __future__ import print_function

import os
import random

from base import BaseTest, main, Timer, TestData
import emcore as emc

Column = emc.Table.Column
Row = emc.Table.Row


class TestTable(BaseTest):

    def createTable(self, nRows):
        table = emc.Table([
            Column(1, "col1", emc.typeSizeT),
            Column(2, "col2", emc.typeFloat),
            Column(3, "col3", emc.typeString)
        ])

        row = table.createRow()

        for i in range(nRows):
            row["col1"] = i;
            row["col2"] = float(i) / 100;
            row["col3"] = "image_%03d" % i
            table.addRow(row);

        return table

    def checkColumns(self, table, indexes=None):
        indexes = indexes or [0, 1, 2]
        colNames = ["col1", "col2", "col3"]

        # Check all expected columns are there
        for i, col in enumerate(table.iterColumns()):
            self.assertEqual(colNames[indexes[i]], col.getName())

    def test_ColumnsBasic(self):
        c1Name = "firstCol"
        c1 = Column(c1Name, emc.typeFloat)
        self.assertEqual(c1.getId(), Column.NO_ID)
        self.assertEqual(c1.getName(), c1Name)
        self.assertEqual(c1.getType(), emc.typeFloat)

        c2Name = "secondCol"
        c2 = Column(c2Name, emc.typeInt16)
        self.assertEqual(c2.getId(), Column.NO_ID)
        self.assertEqual(c2.getName(), c2Name)
        self.assertEqual(c2.getType(), emc.typeInt16)

        colMap = emc.Table()
        self.assertEqual(0, colMap.addColumn(c1));
        self.assertEqual(1, colMap.addColumn(c2));

        # Get columns stored and do some validations
        rc1 = colMap.getColumnByIndex(0)
        rc2 = colMap.getColumnByIndex(1)
        self.assertEqual(rc1.getId(), 1)
        self.assertEqual(rc2.getId(), 2)

        self.assertEqual(0, colMap.getIndex(c1.getName()))
        self.assertEqual(1, colMap.getIndex(c2.getName()))

        self.assertEqual(0, colMap.getIndex(rc1.getId()))
        self.assertEqual(1, colMap.getIndex(rc2.getId()))

        #self.assertRaises(emc.Error, lambda: colMap.getIndex(100))
        #self.assertRaises(emc.Error, lambda: colMap.getIndex("noColumn"))

        # Add more columns with and without IDs
        bigId = 100
        c3index = colMap.addColumn(Column(bigId, "thirdCol", emc.typeFloat))
        rc3 = colMap.getColumnByIndex(c3index)
        self.assertEqual(bigId, rc3.getId())
        self.assertEqual("thirdCol", rc3.getName())

        colMap.addColumn(Column("forthCol", emc.typeFloat))
        rc4 = colMap.getColumnByIndex(c3index + 1)
        self.assertEqual(bigId + 1, rc4.getId());
        self.assertEqual("forthCol", rc4.getName())

        # Let's insert a new column, all indexes before should not be
        # changed, but the ones after the position should be increased by 1
        c3bindex = colMap.insertColumn(Column("thirdBCol", emc.typeFloat),
                                       c3index + 1)
        self.assertEqual(c3bindex, c3index + 1)

        for i, colName in enumerate(["firstCol", "secondCol", "thirdCol"]):
            self.assertEqual(colMap.getIndex(colName), i)

        self.assertEqual(colMap.getIndex("forthCol"), c3bindex + 1)

    def test_TableRowSetValue(self):
        table = emc.Table([
            Column(1, "col1", emc.typeFloat),
            Column(2, "col2", emc.typeInt16),
            Column(3, "col3", emc.typeString)
        ])

        for i, t in enumerate([(1, "col1", emc.typeFloat),
                               (2, "col2", emc.typeInt16),
                               (3, "col3", emc.typeString)]):
            col = table.getColumnByIndex(i)
            self.assertEqual(t[0], col.getId())
            self.assertEqual(t[1], col.getName())
            self.assertEqual(t[2], col.getType())

        row = table.createRow()

        print("Row (before set) >>> ", row)

        # Define some constants values to set and check
        NAMES = ['My name', 'Second name', 'Yet another name']
        FLOATS = [3.1416]
        INTS = [300, 400]

        row[1] = FLOATS[0]
        row[2] = INTS[0]
        row[3] = NAMES[0]

        print("Adding row to table...")
        table.addRow(row);

        print("Accessing row by index. ")
        row2 = table[0]


    def test_TableBasic(self):
        table = emc.Table([
            Column(1, "col1", emc.typeFloat),
            Column(2, "col2", emc.typeInt16),
            Column(3, "col3", emc.typeString)
        ])

        for i, t in enumerate([(1, "col1", emc.typeFloat),
                               (2, "col2", emc.typeInt16),
                               (3, "col3", emc.typeString)]):
            col = table.getColumnByIndex(i)
            self.assertEqual(t[0], col.getId())
            self.assertEqual(t[1], col.getName())
            self.assertEqual(t[2], col.getType())

        row = table.createRow()

        print("Row (before set) >>> ", row)

        # Define some constants values to set and check
        NAMES = ['My name', 'Second name', 'Yet another name']
        FLOATS = [3.1416]
        INTS = [300, 400]

        row[1] = FLOATS[0]
        row[2] = INTS[0]
        row[3] = NAMES[0]

        print("Row (after set) >>> ", row)

        # Check values are properly set
        self.assertAlmostEqual(float(row[1]), FLOATS[0], 3)
        self.assertEqual(int(row[2]), INTS[0])
        self.assertEqual(str(row[3]), NAMES[0])

        # Check we get same values using Column IDs or Names
        self.assertEqual(row[1], row["col1"])
        self.assertEqual(row[2], row["col2"])
        self.assertEqual(row[3], row["col3"])

        # Check we can retrieve values in normal variables
        x = int(row[2])
        self.assertEqual(x, INTS[0])

        # Lets add another row, based on first one, but with some changes
        row2 = Row(row)
        row2["col2"] = INTS[1]
        row2["col3"] = NAMES[1]

        x = int(row2[2])
        self.assertEqual(x, INTS[1])

        print("Row2 >>> ", row2)

        # Check that row was not modified after the copy
        self.assertAlmostEqual(float(row[1]), FLOATS[0], 3)
        self.assertEqual(int(row[2]), INTS[0])
        self.assertEqual(str(row[3]), NAMES[0])

        # Let's add both rows to the table
        table.addRow(row);
        table.addRow(row2);

        for row in table:
            row["col3"] = NAMES[2]
            row["col2"] = int(row["col2"]) / 10

        print("Table: ", table)

        self.assertEqual(2, table.getSize())
        self.assertFalse(table.isEmpty())

        # Check row values are as expected (using iterator)
        for i, row in enumerate(table):
            self.assertAlmostEqual(row["col1"], FLOATS[0], 3)
            self.assertEqual(row["col2"], INTS[i]/10)
            self.assertEqual(row["col3"], NAMES[2])

        # Check row values are as expected (using indexing)
        for i in range(table.getSize()):
            row = table[i]
            self.assertAlmostEqual(row["col1"], FLOATS[0], 3)
            self.assertEqual(row["col2"], INTS[i]/10)
            self.assertEqual(row["col3"], NAMES[2])

        table.clear()
        self.assertEqual(0, table.getSize())
        self.assertTrue(table.isEmpty())

    def test_Copy(self):
        n = 10
        table10 = self.createTable(n)
        table10copy = emc.Table(table10)
        self.checkColumns(table10copy)
        self.assertEqual(table10copy.getSize(), table10.getSize());
        self.assertEqual(table10copy.getSize(), n);

        colNames = [col.getName() for col in table10.iterColumns()]

        for i in range(n):
            row1 = table10[i];
            row2 = table10copy[i];
            for cn in colNames:
                self.assertEqual(row1[cn], row2[cn]);
                
    def test_RemoveColumns(self):
        # Let's create a table with no rows
        table0 = self.createTable(0)
        self.assertEqual(table0.getSize(), 0)
        self.assertTrue(table0.isEmpty())

        # Check all expected columns are there
        self.checkColumns(table0)
        # Remove a column from empty table works
        table0.removeColumn("col2")
        self.checkColumns(table0, [0, 2])
        #
        table10 = self.createTable(10)
        self.checkColumns(table10)
        table10.removeColumn("col2")
        self.checkColumns(table10, [0, 2])

    def test_Read(self):
        td = TestData()
        self.assertTrue(emc.TableFile.hasImpl('star'))

        root = td.get("relion_tutorial/import/")
        fn1 = root + "case1/classify3d_small_it038_data.star"
        print("Reading star: ", fn1)

        tio = emc.TableFile()
        tio.open(fn1)
        tableNames = tio.getTableNames()
        print("Tables: ", tableNames)
        tio.close()
        t = emc.Table()
        t.read(fn1)  # should read first table by default

        t = emc.Table([
            Column(1, "col1", emc.typeFloat),
            Column(2, "col2", emc.typeInt16),
            Column(3, "col3", emc.typeString)
        ])

        self.assertEqual(t.getColumnsSize(), 3);
        self.assertTrue(t.isEmpty());

        t.read("images", fn1)
        print("Size: ", t.getSize())
        self.assertEqual(t.getSize(), 79)

        refColNames = [
            "rlnVoltage", "rlnDefocusU", "rlnSphericalAberration",
            "rlnAmplitudeContrast", "rlnImageName", "rlnNormCorrection",
            "rlnMicrographName", "rlnGroupNumber", "rlnOriginX",
            "rlnOriginY", "rlnAngleRot", "rlnAngleTilt", "rlnAnglePsi",
            "rlnClassNumber", "rlnLogLikeliContribution",
            "rlnNrOfSignificantSamples", "rlnMaxValueProbDistribution"
        ]
        """
        loop_
_rlnVoltage #1
_rlnDefocusU #2
_rlnSphericalAberration #3
_rlnAmplitudeContrast #4
_rlnImageName #5
_rlnNormCorrection #6
_rlnMicrographName #7
_rlnGroupNumber #8
_rlnOriginX #9
_rlnOriginY #10
_rlnAngleRot #11
_rlnAngleTilt #12
_rlnAnglePsi #13
_rlnClassNumber #14
_rlnLogLikeliContribution #15
_rlnNrOfSignificantSamples #16
_rlnMaxValueProbDistribution #17
  300.000000 15060.500000     2.260000     0.100000 000080@cluster/wind2/win_05677.dat
  0.771391 cluster/wind2/win_00849.dat            1    -2.968653   -22.218653   -20.283924
  175.359562   126.884867            5 24711.149348            1     0.962371
        """
        goldValues = [
            ('rlnGroupNumber',
            [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
             17, 18, 19, 20, 21]),
            ('rlnDefocusU',
             [15060.5, 16009.5, 18642.2, 24642.4, 24772.8, 25580.8, 26833.9, 27164.9,
              30135.1, 33744.2, 35465.5, 13923.4, 17895.1, 19206.2, 24165.1,
              24650.4, 25866.7, 26527.1, 27915.1, 29939.5, 30062.1]),
            ('rlnImageName',
             ['000080@cluster/wind2/win_05677.dat',
              '000023@cluster/wind2/win_05883.dat', '000042@cluster/wind2/win_04377.dat',
              '000065@cluster/wind2/win_00139.dat', '000038@cluster/wind2/win_04884.dat',
              '000046@cluster/wind2/win_02565.dat', '000023@cluster/wind2/win_00161.dat',
              '000026@cluster/wind2/win_03758.dat', '000058@cluster/wind2/win_03862.dat',
              '000024@cluster/wind2/win_04246.dat', '000073@cluster/wind2/win_02382.dat',
              '000021@cluster/wind2/win_04587.dat', '000007@cluster/wind2/win_03186.dat',
              '000056@cluster/wind2/win_04919.dat', '000076@cluster/wind2/win_03268.dat',
              '000048@cluster/wind2/win_04346.dat', '000073@cluster/wind2/win_03740.dat',
              '000036@cluster/wind2/win_04583.dat', '000015@cluster/wind2/win_05453.dat',
              '000021@cluster/wind2/win_00846.dat', '000080@cluster/wind2/win_04165.dat'])
        ]

        for i, col in enumerate(t.iterColumns()):
            self.assertEqual(refColNames[i], col.getName())
            print(col)

        for label, values in goldValues:
            mdValues = [row[label] for i, row in enumerate(t) if i < 20]
            for v1, v2 in zip(values, mdValues):
                self.assertEqual(v2, v1)

    def test_ReadSingleRow(self):
        self.assertTrue(emc.TableFile.hasImpl('star'))

        td = TestData()
        root = td.get("relion_tutorial/import/")

        fn1 = root + "case1/classify3d_small_it038_optimiser.star"
        print("Reading star: ", fn1)

        t = emc.Table()
        t.read("optimiser_general", fn1)
        self.assertEqual(t.getColumnsSize(), 53)
        self.assertEqual(t.getSize(), 1)

    def test_ReadXmd(self):
        self.assertTrue(emc.TableFile.hasImpl('star'))

        td = TestData()

        fn1 = td.get("xmipp_tutorial/gold/images200k.xmd")
        print("Reading xmd: ", fn1)

        tio = emc.TableFile()
        table = emc.Table()

        t = Timer()
        t.tic()

        tio.open(fn1)
        tio.read("noname", table)

        t.toc()

        print("Size: ", table.getSize())
        tio.close()

        refColNames = ['itemId', 'xcoor', 'ycoor', 'image', 'micrograph',
                       'enabled', 'ctfDefocusU', 'ctfDefocusV',
                       'ctfDefocusAngle', 'ctfQ0', 'ctfSphericalAberration',
                       'ctfVoltage', 'zScore', 'zScoreShape1', 'zScoreShape2',
                       'zScoreSNR1', 'zScoreSNR2', 'zScoreHistogram']

        # Get column by index
        for i in range(table.getColumnsSize()):
            col = table.getColumnByIndex(i)
            self.assertEqual(refColNames[i], col.getName())

        # Iterate through the columns
        for i, col1 in enumerate(table.iterColumns()):
            col2 = table.getColumnByIndex(i)
            self.assertEqual(refColNames[i], col1.getName())
            self.assertEqual(refColNames[i], col2.getName())
            self.assertEqual(col1, col2)


    def test_Sort(self):
        N = 100000

        t = self.createTable(N)
        v = [random.uniform(1.0, 10.0) for i in range(N)]

        for i, row in enumerate(t):
            row["col2"] = v[i]

        timer = Timer()
        timer.tic()
        v.sort()
        timer.toc("Sorted vector")

        timer.tic();
        t.sort(["col2"]);
        timer.toc("Sorted table by float")

        for i, row in enumerate(t):
            self.assertAlmostEqual(row['col2'], v[i])

        timer.tic(); # Sort by string column
        t.sort(["col3"]);
        timer.toc("Sorted table by string");

        # Sorting descendent
        t.sort(["col2 DESC"]);
        for i, row in enumerate(t):
            self.assertAlmostEqual(row['col2'], v[N-i-1])



if __name__ == '__main__':
    main()


