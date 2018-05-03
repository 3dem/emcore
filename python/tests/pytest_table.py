
from base import BaseTest, main

import em

Column = em.Table.Column
Row = em.Table.Row


class TestTable(BaseTest):

    def test_ColumnsBasicbasic(self):

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

        #
        # for (auto& row: table)
        # {
        #     row["col3"] = std::string("Other name 2");
        # row["col2"] = (int)row["col2"] / 10;
        # }
        #
        # printTable(table);
        #
        # self.assertEqual(table.getSize(), 2);
        # ASSERT_FALSE(table.isEmpty());
        #
        # table.clear();
        # self.assertEqual(table.getSize(), 0);
        # ASSERT_TRUE(table.isEmpty());


if __name__ == '__main__':
    main()


