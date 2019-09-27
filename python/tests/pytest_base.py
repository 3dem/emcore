
from base import BaseTest, main

import emcore as emc


class TestType(BaseTest):
    def test_basic(self):
        t1 = emc.Type()
        self.assertTrue(t1.isNull())
        self.assertEqual(t1.getSize(), 0)

        t2 = emc.typeInt8
        self.assertFalse(t2.isNull())
        self.assertEqual(t2.getSize(), 1)
        self.assertEqual(t2.getName(), "int8")


class TestArrayDim(BaseTest):

    def test_init(self):
        adim = emc.ArrayDim()
        self.assertEqual(adim, emc.ArrayDim(0, 1, 1, 1))

        adim = emc.ArrayDim(100)
        self.assertEqual(adim, emc.ArrayDim(100, 1, 1, 1))

        adim = emc.ArrayDim(100, 100, 1, 100)
        self.assertEqual(adim.getSize(), 100**3)
        self.assertEqual(adim.getItemSize(), 100 ** 2)

        # Test copy of ArrayDim and its post-conditions
        adim2 = emc.ArrayDim(adim)
        self.assertEqual(adim, adim2)
        self.assertEqual(adim2.getSize(), 100 ** 3)
        self.assertEqual(adim2.getItemSize(), 100 ** 2)

        # Let's modify the copy and
        # make sure the original is not changed
        adim2.x = 10
        adim2.y = 10
        adim2.n = 10
        self.assertEqual(adim.getSize(), 100 ** 3)
        self.assertEqual(adim.getItemSize(), 100 ** 2)
        self.assertEqual(adim2.getSize(), 10 ** 3)
        self.assertEqual(adim2.getItemSize(), 10 ** 2)


class TestArray(BaseTest):
    def test_init(self):
        a1 = emc.Array()

        adim = emc.ArrayDim(10, 10)
        A = emc.Array(adim, emc.typeFloat)

        import numpy as np
        a = np.array(A, copy=False)
        print("shape", a.shape)

        a[:, :] = 0

        print(a)

        """
            ArrayDim adim(10, 10);
    Array A(adim, typeInt32);
    ArrayView<int> Av = A.getView<int>();

    Av.assign(11);
    Av(3, 3) = 20;
    Av(4, 4) = 20;
    Av(5, 5) = 20;
    int * ptr = Av.getPointer();
    ptr[10] = 15;

    //std::cout << Av.toString() << std::endl;

    Array A2(A);
    ArrayView<int> Av2 = A2.getView<int>();
    const int * data2 = Av2.getPointer();
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data2[i], ptr[i]);

    Array A3(adim, typeFloat);
    A3.copy(A);
    ASSERT_EQ(A3.getType(), typeFloat);
    auto data3f = static_cast<const float *>(A3.getPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3f[i], (float)ptr[i]);

    A3.copy(A, typeUInt32);
    ASSERT_EQ(A3.getType(), typeUInt32);
    auto data3ui = static_cast<const uint32_t *>(A3.getPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data3ui[i], (uint32_t)ptr[i]);

    Array A4;
    A4.copy(A);
    ASSERT_EQ(A4.getType(), typeInt32);
    auto data3i = static_cast<const int32_t *>(A3.getPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3i[i], (int32_t)ptr[i]);
        """


if __name__ == '__main__':
    main()


