
from base import BaseTest, main

import emcore as emc


class TestType(BaseTest):
    
    def test_basic(self):
        # Test basic properties of Type singleton instances
        n = emc.typeNull
        self.assertEqual(n.getName(), "")
        self.assertEqual(n.getSize(), 0)
        self.assertEqual(n, emc.Object().getType())

        i = emc.typeInt32
        self.assertEqual(i.getName(), "int32")
        self.assertTrue(i.isPod())
        self.assertEqual(i.getSize(), 4)
        #FIXME: WHY THE FOLLOWING ASSERT FAILS????
        #self.assertEqual(n, emc.Object(1).getType())
        print(i)

        f = emc.typeFloat
        self.assertEqual(f.getName(), "float")
        self.assertTrue(f.isPod())
        self.assertEqual(f.getSize(), 4)
        #self.assertEqual(n, emc.Object(1.0).getType())
        print(f)
    
        d = emc.typeDouble
        self.assertEqual(d.getName(), "double")
        self.assertTrue(d.isPod())
        self.assertEqual(d.getSize(), 8)
        print(d)

        i8 = emc.typeInt8
        self.assertEqual(i8.getName(), "int8")
        self.assertTrue(i8.isPod())
        self.assertEqual(i8.getSize(), 1)
        print(i8)
        
    def test_infer(self):
        self.assertTypeEqual(emc.Type.inferFromString("100"), emc.typeInt32)
        self.assertTypeEqual(emc.Type.inferFromString("100.00"), emc.typeFloat)
        self.assertTypeEqual(emc.Type.inferFromString("192.168.100.1"), emc.typeString)
        self.assertTypeEqual(emc.Type.inferFromString("/this/is/a/path"), emc.typeString)
        line = "100 3.4 4.5 abc"
        self.assertTypeEqual(emc.Type.inferFromString(line), emc.typeString)
        # self.assertTypeEqual(emc.Type.inferFromString(ptr, 3), emc.typeInt32)
        # self.assertTypeEqual(emc.Type.inferFromString(ptr+4, 3), emc.typeFloat)
        # self.assertTypeEqual(emc.Type.inferFromString(ptr+8, 3), emc.typeFloat)
        # self.assertTypeEqual(emc.Type.inferFromString(ptr+12, 3), emc.typeString)


if __name__ == '__main__':
    main()


