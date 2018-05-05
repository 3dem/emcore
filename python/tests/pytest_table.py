
import os

from base import BaseTest, main, Timer
import em

Column = em.Table.Column
Row = em.Table.Row


class TestTable(BaseTest):

    def createTable(self, nRows):
        table = em.Table([
            Column(1, "col1", em.typeSizeT),
            Column(2, "col2", em.typeFloat),
            Column(3, "col3", em.typeString)
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
        for i in range(table.getColumnsSize()):
            self.assertEqual(colNames[indexes[i]],
                             table.getColumnByIndex(i).getName())

    def test_ColumnsBasic(self):
        c1Name = "firstCol"
        c1 = Column(c1Name, em.typeFloat)
        self.assertEqual(c1.getId(), Column.NO_ID)
        self.assertEqual(c1.getName(), c1Name)
        self.assertEqual(c1.getType(), em.typeFloat)

        c2Name = "secondCol"
        c2 = Column(c2Name, em.typeInt16)
        self.assertEqual(c2.getId(), Column.NO_ID)
        self.assertEqual(c2.getName(), c2Name)
        self.assertEqual(c2.getType(), em.typeInt16)

        colMap = em.Table()
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

        #self.assertRaises(em.Error, lambda: colMap.getIndex(100))
        #self.assertRaises(em.Error, lambda: colMap.getIndex("noColumn"))

        # Add more columns with and without IDs
        bigId = 100
        c3index = colMap.addColumn(Column(bigId, "thirdCol", em.typeFloat))
        rc3 = colMap.getColumnByIndex(c3index)
        self.assertEqual(bigId, rc3.getId())
        self.assertEqual("thirdCol", rc3.getName())

        colMap.addColumn(Column("forthCol", em.typeFloat))
        rc4 = colMap.getColumnByIndex(c3index + 1)
        self.assertEqual(bigId + 1, rc4.getId());
        self.assertEqual("forthCol", rc4.getName())

        # Let's insert a new column, all indexes before should not be
        # changed, but the ones after the position should be increased by 1
        c3bindex = colMap.insertColumn(Column("thirdBCol", em.typeFloat),
                                       c3index + 1)
        self.assertEqual(c3bindex, c3index + 1)

        for i, colName in enumerate(["firstCol", "secondCol", "thirdCol"]):
            self.assertEqual(colMap.getIndex(colName), i)

        self.assertEqual(colMap.getIndex("forthCol"), c3bindex + 1)

    def test_TableBasic(self):
        table = em.Table([
            Column(1, "col1", em.typeFloat),
            Column(2, "col2", em.typeInt16),
            Column(3, "col3", em.typeString)
        ])

        for i, t in enumerate([(1, "col1", em.typeFloat),
                               (2, "col2", em.typeInt16),
                               (3, "col3", em.typeString)]):
            col = table.getColumnByIndex(i)
            self.assertEqual(t[0], col.getId())
            self.assertEqual(t[1], col.getName())
            self.assertEqual(t[2], col.getType())

        row = table.createRow()

        print("Row (before set) >>> ", row)

        row[1] = 3.1416
        row[2] = 300
        row[3] = "My name"

        print("Row (after set) >>> ", row)


        self.assertAlmostEqual(float(row[1]), 3.1416, 3)
        self.assertEqual(int(row[2]), 300)
        self.assertEqual(str(row[3]), "My name")

        self.assertEqual(row[1], row["col1"])
        self.assertEqual(row[2], row["col2"])
        self.assertEqual(row[3], row["col3"])

        x = int(row[2])
        self.assertEqual(x, 300)

        row3 = Row(row)
        row3["col2"] = 400
        row3["col3"] = "Other name"
        x = int(row3[2])
        self.assertEqual(x, 400)

        print("Row3 >>> ", row3)

        # Check that row was not modified after the copy
        self.assertAlmostEqual(float(row[1]), 3.1416, 3)
        self.assertEqual(int(row[2]), 300)
        self.assertEqual(str(row[3]), "My name")


        table.addRow(row);
        table.addRow(row3);

        # FIXME: Add binding for the iterator
        # for row in table:
        for i in range(table.getSize()):
            row = table[i]
            row["col3"] = "Other name 2"
            row["col2"] = int(row["col2"]) / 10

        print("Table: ", table)

        self.assertEqual(2, table.getSize())
        self.assertFalse(table.isEmpty())

        table.clear()
        self.assertEqual(0, table.getSize())
        self.assertTrue(table.isEmpty())

    def test_Copy(self):
        n = 10
        table10 = self.createTable(n)
        table10copy = em.Table(table10)
        self.checkColumns(table10copy)
        self.assertEqual(table10copy.getSize(), table10.getSize());
        self.assertEqual(table10copy.getSize(), n);

        colNames = [table10.getColumnByIndex(i).getName()
                    for i in range(table10.getColumnsSize())]

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
        testDataPath = os.environ.get("EM_TEST_DATA", None)

        self.assertTrue(em.TableIO.hasImpl('star'))

        if testDataPath is not None:
            root = testDataPath + "relion_tutorial/import/"
            fn1 = root + "case1/classify3d_small_it038_data.star";
            print("Reading star: ", fn1)

            t = em.Table([
                Column(1, "col1", em.typeFloat),
                Column(2, "col2", em.typeInt16),
                Column(3, "col3", em.typeString)
            ])

            tio = em.TableIO()

            self.assertEqual(t.getColumnsSize(), 3);
            self.assertTrue(t.isEmpty());

            tio.open(fn1)
            tio.read("images", t)

            refColNames = [
                "rlnVoltage", "rlnDefocusU", "rlnSphericalAberration",
                "rlnAmplitudeContrast", "rlnImageName", "rlnNormCorrection",
                "rlnMicrographName", "rlnGroupNumber", "rlnOriginX",
                "rlnOriginY", "rlnAngleRot", "rlnAngleTilt", "rlnAnglePsi",
                "rlnClassNumber", "rlnLogLikeliContribution",
                "rlnNrOfSignificantSamples", "rlnMaxValueProbDistribution"
            ]

            for i in range(t.getColumnsSize()):
                col = t.getColumnByIndex(i)
                self.assertEqual(refColNames[i], col.getName())

    def test_ReadXmd(self):
        testDataPath = os.environ.get("EM_TEST_DATA", None)

        self.assertTrue(em.TableIO.hasImpl('star'))

        if testDataPath is not None:
            fn1 = testDataPath + "xmipp_tutorial/gold/images200k.xmd"
            print("Reading xmd: ", fn1)

            tio = em.TableIO()
            table = em.Table()

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
            
            for i in range(table.getColumnsSize()):
                col = table.getColumnByIndex(i)
                self.assertEqual(refColNames[i], col.getName())

if __name__ == '__main__':
    main()


