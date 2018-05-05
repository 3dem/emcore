
from base import BaseTest, main

import em


class TestObject(BaseTest):
    def test_basic(self):
        # Empty object constructor
        eo = em.Object()
        # After this, the object type should be Null
        self.assertEqual(eo.getType(), em.typeNull)
        self.assertTrue(eo.getType().isNull())

        eo.set(1) # Now this object should change its type to typeInt32
        self.assertEqual(eo.getType(), em.typeInt32)
        # When the object type is not null, it should not
        # change its type and just try to convert to the underlying type.
        eo.set(2.2)
        self.assertEqual(eo.getType(), em.typeInt32)
        self.assertEqual(int(eo), int(2.2))

        # Copy constructor
        o = em.Object(1)
        self.assertEqual(o.getType(), em.typeInt32)
        x = int(o)
        self.assertEqual(x, 1)
        o = 2
        self.assertEqual(int(o), 2)

        o2 = em.Object(3.5) # Type should be double
        self.assertEqual(o2.getType(), em.typeFloat)
        # We can explicitly change the type of a given object
        o2.setType(em.typeDouble)
        o2.set(1.3)
        self.assertEqual(o2.getType(), em.typeDouble)
        f = float(o2)
        self.assertAlmostEqual(f, 1.3)

    # o2 = 5.6f;
    #
    # float d, d2 = 5.6 + float(o2)
    # float d3;
    #
    # size_t N = 100;
    # float values [] = {1.5f, 2.3f, 5.7f, 3.2f, 10.f, 56.f};
    # std::vector<Object> vobj;
    #
    # for (int i = 0; i < N; i++)
    # {
    #     d = values[i % 6];
    # vobj.push_back(Object(d))
    # }
    #
    # for (int i = 0; i < N; ++i)
    # {
    #     d = vobj[i];
    # ASSERT_FLOAT_EQ(d, values[i % 6])
    # }
    #
    # const char * str = "This is a test string";
    #
    # Object o3;
    # o3 = std::string(str)
    # std::string s2 = o3;
    # self.assertEqual(s2, str)
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


