
from base import BaseTest, main

import emcore as emc


class TestObject(BaseTest):
    def test_basic(self):
        # Empty object constructor
        eo = emc.Object()
        # After this, the object type should be Null
        self.assertEqual(eo.getType(), emc.typeNull)
        self.assertTrue(eo.getType().isNull())

        eo.set(1) # Now this object should change its type to typeInt32
        self.assertEqual(eo.getType(), emc.typeInt32)
        # When the object type is not null, it should not
        # change its type and just try to convert to the underlying type.
        eo.set(2.2)
        self.assertEqual(eo.getType(), emc.typeInt32)
        self.assertEqual(int(eo), int(2.2))

        # Copy constructor
        o = emc.Object(1)
        self.assertEqual(o.getType(), emc.typeInt32)
        x = int(o)
        self.assertEqual(x, 1)
        o = 2
        self.assertEqual(int(o), 2)

        o2 = emc.Object(3.5) # Type should be double
        self.assertEqual(o2.getType(), emc.typeFloat)
        # We can explicitly change the type of a given object
        o2.setType(emc.typeDouble)
        o2.set(1.3)
        self.assertEqual(o2.getType(), emc.typeDouble)
        f = float(o2)
        self.assertAlmostEqual(f, 1.3)

        o2 = emc.Object(5.6);
        d2 = 5.6 + float(o2)
        N = 100
        values = [1.5, 2.3, 5.7, 3.2, 10., 56.]
        vobj = []

        for i in range(N):
            vobj.append(emc.Object(values[i % 6]))

        for i, obj in enumerate(vobj):
            self.assertAlmostEqual(values[i % 6], float(obj), 5)

        s1 = "This is a test string";
        o3 = emc.Object(s1)

        s2 = str(o3)
        self.assertEqual(s1, s2)

        # img = emc.Image(emc.ArrayDim(10, 10), emc.typeFloat)
        # o3 = emc.Object(img)

    #
    # Image img(ArrayDim(10, 10), typeFloat)
    # auto& typeImage = Type::get<Image>()
    # o3.setType(typeImage)
    # o3 = img;
    # self.assertEqual(o3.getType(), typeImage)
    # Image img2 = o3;
    #
    # # Test copy and equality
    # Object oo3 = std::string("One string")
    # Object oo2 = oo3;
    # self.assertEqual(oo2, oo3)
    #
    # Object o4(oo3)
    # self.assertEqual(o4, oo3)
    #
    # ObjectVector ov;
    # ov.resize(3)
    #
    # ov[0] = oo3;
    # self.assertEqual(ov[0], oo3)
    # self.assertEqual(ov[0], oo2)
    # //self.assertEqual((std::string) o2, (std::string) o3)
    # Object o5(oo2)
    # self.assertEqual(oo2, o5)
    #
    # # Test copy objects of different types
    # Object oInt = 100;
    # Object oStr = std::string("testing")
    # oInt = oStr;



if __name__ == '__main__':
    main()


