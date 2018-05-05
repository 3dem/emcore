
from base import BaseTest, main

import em


class TestType(BaseTest):
    
    def test_basic(self):
        # Test basic properties of Type singleton instances
        n = em.typeNull
        self.assertEqual(n.getName(), "")
        self.assertEqual(n.getSize(), 0)
        self.assertEqual(n, em.Object().getType())

        i = em.typeInt32
        self.assertEqual(i.getName(), "int32")
        self.assertTrue(i.isPod())
        self.assertEqual(i.getSize(), 4)
        #FIXME: WHY THE FOLLOWING ASSERT FAILS????
        #self.assertEqual(n, em.Object(1).getType())
        print(i)

        f = em.typeFloat
        self.assertEqual(f.getName(), "float")
        self.assertTrue(f.isPod())
        self.assertEqual(f.getSize(), 4)
        #self.assertEqual(n, em.Object(1.0).getType())
        print(f)
    
        d = em.typeDouble
        self.assertEqual(d.getName(), "double")
        self.assertTrue(d.isPod())
        self.assertEqual(d.getSize(), 8)
        print(d)

        i8 = em.typeInt8
        self.assertEqual(i8.getName(), "int8")
        self.assertTrue(i8.isPod())
        self.assertEqual(i8.getSize(), 1)
        print(i8)
        
    def test_infer(self):
        self.assertTypeEqual(em.Type.inferFromString("100"), em.typeInt32)
        self.assertTypeEqual(em.Type.inferFromString("100.00"), em.typeFloat)
        self.assertTypeEqual(em.Type.inferFromString("192.168.100.1"), em.typeString)
        self.assertTypeEqual(em.Type.inferFromString("/this/is/a/path"), em.typeString)
        line = "100 3.4 4.5 abc"
        self.assertTypeEqual(em.Type.inferFromString(line), em.typeString)
        # self.assertTypeEqual(em.Type.inferFromString(ptr, 3), em.typeInt32)
        # self.assertTypeEqual(em.Type.inferFromString(ptr+4, 3), em.typeFloat)
        # self.assertTypeEqual(em.Type.inferFromString(ptr+8, 3), em.typeFloat)
        # self.assertTypeEqual(em.Type.inferFromString(ptr+12, 3), em.typeString)


if __name__ == '__main__':
    main()


