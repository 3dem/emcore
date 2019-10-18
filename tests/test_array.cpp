//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "emc/base/error.h"
#include "emc/base/image.h"
#include "emc/base/legacy.h"


using namespace emcore;

TEST(ArrayDim, Defaults) {

    // Default ctor of ArrayDim should set everything to 0
    ArrayDim adim;
    ASSERT_TRUE((adim == ArrayDim(0, 1, 1, 1)));

    // When at least one of the dimensions is set
    // the rest will be set to 1
    ArrayDim adim2(100);
    ASSERT_TRUE((adim2 == ArrayDim(100, 1, 1, 1)));
    ASSERT_EQ(adim2.getRank(), 1);

    // Test helper functions to compute the size
    ArrayDim adim3(100, 100, 1, 100);

    ASSERT_EQ(adim3.getSize(), 100 * 100 * 100);
    ASSERT_EQ(adim3.getItemSize(), 100 * 100);
    ASSERT_EQ(adim3.getRank(), 2);

    // Test copy constructor
    ArrayDim adim4(adim3);

    ASSERT_EQ(adim4, adim3);
    ASSERT_EQ(adim4.getSize(), 100 * 100 * 100);
    ASSERT_EQ(adim4.getItemSize(), 100 * 100);
    ASSERT_EQ(adim4.getRank(), 2);

    adim4.z = 100;  // After this rank should be 3
    ASSERT_EQ(adim4.getRank(), 3);


} // TEST(ArrayTest, ArrayDim)


TEST(Array, Basic) {

    ArrayDim adim(10, 10);
    Array A(adim, typeInt32);
    ArrayT<int> Av = A.getView<int>();

    // Check the assignment through the ArrayT class
    Av.assign(11);
    int * ptr = Av.getData();

    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(11, ptr[i]);

    // Now test the assignment through the Array class directly
    A.set(22);
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(22, ptr[i]);

    Array A0(A);
    A0 += 22;
    int * ptr0 = static_cast<int*>(A0.getData());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(44, ptr0[i]);

    A0 -= A; // Now A0 and A should be equal
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(ptr[i], ptr0[i]);

    A0 /= 22; // now all values are 1
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(1, ptr0[i]);

    A0 *= A; // equals again
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(ptr[i], ptr0[i]);

    A0 += A;
    A0 += A0; // Now A0 is 4 times A
    A0 /= 4;
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(ptr[i], ptr0[i]);

    Av(3, 3) = 20;
    Av(4, 4) = 20;
    Av(5, 5) = 20;

    ptr[10] = 15;

    Array A2(A);
    ArrayT<int> Av2 = A2.getView<int>();
    const int * data2 = Av2.getData();
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data2[i], ptr[i]);

    Array A3(adim, typeFloat);
    A3.copy(A);
    ASSERT_EQ(A3.getType(), typeFloat);
    auto data3f = static_cast<const float *>(A3.getData());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3f[i], (float)ptr[i]);

    // Test the resize based on another Array dimensions and type
    A3.resize(A);
    A3 = A;
    ASSERT_EQ(A3.getType(), typeInt32);
    auto data3ui = static_cast<const int32_t *>(A3.getData());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data3ui[i], (int32_t)ptr[i]);

    Array A4;
    A4 = A;
    ASSERT_EQ(A4.getType(), typeInt32);
    auto data3i = static_cast<const int32_t *>(A3.getData());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3i[i], (int32_t)ptr[i]);

    // Check that the resize operation will not allocate new memory
    // if the currently allocated one is enough to store new type and dimensions
    void * adata1 = A.getData();

    A.resize(adim, typeUInt32);  // typeInt32 and typeUInt32 should have same size
    void * adata2 = A.getData();
    ASSERT_EQ(adata1, adata2);
    // The memory should also be reused if the same number of elements
    A.resize(ArrayDim(25, 4)); // same number of elements as 10 x 10
    adata1 = A.getData();
    ASSERT_EQ(adata1, adata2);

    // Test empty ctor
    auto nullType = Type();
    Array empty;
    ASSERT_EQ(empty.getType(), nullType);
    ASSERT_EQ(empty.getDim(), ArrayDim(0, 1, 1, 1));
    ASSERT_EQ(empty.getData(), nullptr);

    Array tmp = A; // copy ctor
    ASSERT_EQ(tmp, A);
    empty = std::move(tmp); // move assign
    ASSERT_EQ(empty, A);
    // Now tmp should be empty
    ASSERT_EQ(tmp.getType(), nullType);
    ASSERT_EQ(tmp.getDim(), ArrayDim(0, 1, 1, 1));
    ASSERT_EQ(tmp.getData(), nullptr);

    // Create a new array with same dimensions as A, but with typeFloat
    Array A5(A.getDim(), typeFloat);
    A5 = A;
    // After assignment, then A5 type should be the same of A
    ASSERT_EQ(A5.getType(), A.getType());
    // We can make a copy of A elements but using float
    A5.copy(A, typeFloat);
    ASSERT_EQ(A5.getType(), typeFloat);

} // TEST(ArrayTest, Constructor)


TEST(Array, IndexingAlias)
{
    const size_t DIM = 16; // 128
    const size_t N = 10;
    // Create an Array with 10 elements to test aliasing single images
    ArrayDim adim(DIM, DIM, 1, N);

    // Valid indexes
    ASSERT_TRUE(adim.isValidIndex(8, 8));
    ASSERT_TRUE(adim.isValidIndex(8, 8, 0, 8));
    ASSERT_TRUE(adim.isValidIndex(0, 0));
    ASSERT_TRUE(adim.isValidIndex(0, 0, 0, 8));
    ASSERT_TRUE(adim.isValidIndex(0, 0, 0, 1)); // First pixel of first image
    ASSERT_TRUE(adim.isValidIndex(DIM-1, DIM-1, 0, N)); // Last pixel of last image

    // Invalid indexes
    ASSERT_FALSE(adim.isValidIndex(80, 80));
    ASSERT_FALSE(adim.isValidIndex(8, 8, 2, 8));
    ASSERT_FALSE(adim.isValidIndex(0, 0, 0, 0));
    ASSERT_FALSE(adim.isValidIndex(0, 0, 1, N+1));
    ASSERT_FALSE(adim.isValidIndex(0, 0, 0, -1));
    ASSERT_FALSE(adim.isValidIndex(DIM, DIM, 0, N)); // First pixel of first image

    Array array(adim, typeFloat);

    ArrayDim asdim(adim);
    asdim.n = 1;

    size_t xy = adim.getSliceSize();
    size_t xyz = adim.getItemSize();

    auto avAll = array.getView<float>();
    avAll.assign(0);

    try
    {
        for (size_t i = 1; i <= N; ++i)
        {
            auto arraySingle = array.getView(i);
            ASSERT_TRUE(arraySingle.isView());
            ASSERT_EQ(arraySingle.getDim(), asdim);
            auto avSingle = arraySingle.getView<float>();
            // Set different values for each individual image
            avSingle.assign(i);
        }

        float * data = avAll.getData();

        for (size_t i = 0; i < adim.getSize(); ++i)
        {
            size_t n = (i / xyz) + 1; // start at 1
            size_t r = i % xyz;
            size_t z = r / xy;
            r = r % xy;
            size_t y = r / adim.x;
            size_t x = r % adim.x;

            // Now check that the indexing logic is symmetric
            // and also the assign operation over an alias
            ASSERT_EQ(data[i], avAll(x, y, z, n));
            ASSERT_EQ(data[i], n);
        }
    }
    catch (Error &e)
    {
        std::cerr << e << std::endl;
    }

} // TEST(Array, getView)

TEST(Array, Legacy)
{
    const size_t DIM = 16; // 128
    const size_t N = 10;
    // Create an Array with 10 elements to test aliasing single images
    ArrayDim adim(DIM, DIM, 1, N);
    Array array(adim, typeFloat);
    ArrayDim asdim(adim);
    asdim.n = 1;

    size_t xy = adim.getSliceSize();
    size_t xyz = adim.getItemSize();

    auto avAll = array.getView<float>();
    avAll.assign(0);

    try
    {
        for (size_t i = 1; i <= N; ++i)
        {
            auto arraySingle = array.getView(i);
            ASSERT_EQ(arraySingle.getDim(), asdim);
            auto avSingle = arraySingle.getView<float>();
            // Set different values for each individual image
            avSingle.assign(i);
        }

        float * data = avAll.getData();
        LegacyArray<float> legacyArray(adim, data);

        for (size_t i = 0; i < adim.getItemSize(); ++i)
        {
            size_t r = i % xyz;
            size_t z = r / xy;
            r = r % xy;
            size_t y = r / adim.x;
            size_t x = r % adim.x;

            // Now check that the indexing logic is symmetric
            // and also the assign operation over an alias
            ASSERT_EQ(data[i], avAll(x, y, z, 1));
            ASSERT_EQ(data[i], DIRECT_A3D_ELEM(legacyArray, z, y, x));
            ASSERT_EQ(data[i], A3D_ELEM(legacyArray, z, y, x));
        }
    }
    catch (Error &e)
    {
        std::cerr << e << std::endl;
    }

} // TEST(Array, getView)

TEST(Array, copyFromTo)
{
    size_t DIM = 9; // sudoku test array
    size_t dim = DIM / 3;
    size_t N = DIM * DIM;
    // Create an Array with 10 elements to test aliasing single images
    ArrayDim adim(DIM, DIM);

    std::vector<int16_t > v = {1, 1, 1, 2, 2, 2, 3, 3, 3,
                          1, 1, 1, 2, 2, 2, 3, 3, 3,
                          1, 1, 1, 2, 2, 2, 3, 3, 3,
                          4, 4, 4, 5, 5, 5, 6, 6, 6,
                          4, 4, 4, 5, 5, 5, 6, 6, 6,
                          4, 4, 4, 5, 5, 5, 6, 6, 6,
                          7, 7, 7, 8, 8, 8, 9, 9, 9,
                          7, 7, 7, 8, 8, 8, 9, 9, 9,
                          7, 7, 7, 8, 8, 8, 9, 9, 9};


    auto& t = typeInt16;

    Array a(adim, t);
    t.copy(v.data(), a.getData(), N);

    //std::cout << "array: \n" << a << std::endl;

    Array a1(ArrayDim(dim, dim), t);

    auto data = static_cast<const int16_t *>(a.getData());
    a1.set(10);

    // Let's test patch
    a.patch(a1);
    a.patch(a1, 0, 6);

    //std::cout << "array (after): \n" << a << std::endl;

    int16_t goldValue;

    for (int i = 0; i < N; ++i)
    {
        goldValue = (v[i] == 1 or v[i] == 7)? 10 : v[i];
        ASSERT_EQ(data[i], goldValue);
    }

    // Test out of bounds conditions
    EXPECT_THROW(a.patch(a1, 0, 7), Error);
    EXPECT_THROW(a.patch(a1, 7, 0), Error);

    ArrayDim adim2(DIM+1, DIM+1);
    Array a2(adim2, typeFloat);

    a2.set(0);
    // Test that it also works when casting the elements
    a2.patch(a, 1, 1);
    auto data2 = static_cast<const float*>(a2.getData());

    for (int i = 0; i < N; ++i)
    {
        int y = i / DIM + 1;
        int x = i % DIM + 1;
        int j = y * (DIM + 1) + x;

        ASSERT_EQ(data[i], (int)data2[j]);
    }

// Test the other way around extract
    Array b1(a1);
    Array b2 = b1;
    b1.set(0);
    b2.set(0);
    ASSERT_NE(a1, b1);
    ASSERT_NE(a1, b2);

    for (int y = 0; y < DIM; y += dim)
        for (int x = 0; x < DIM; x += dim)
        {
            b1.extract(a, x, y);
            b2.set(data[y * DIM + x]);
            ASSERT_EQ(b1, b2);
        }

    b1.resize(ArrayDim(DIM, 2));
    b1.extract(a, 0, 5);
    auto b1Data = static_cast<const int16_t *>(b1.getData());

    for (int y = 5; y < 7; y ++)
        for (int x = 0; x < DIM; x++)
            ASSERT_EQ(data[y * DIM + x], b1Data[(y-5) * DIM + x]);

    // Create an alias of b1 but in column format
    auto b1Alias = Array(ArrayDim(2, DIM), t, b1.getData());
    b1Alias.extract(a, 5, 0);

    for (int y = 0; y < DIM; y ++)
        for (int x = 5; x < 7; x++)
            ASSERT_EQ(data[y * DIM + x], b1Data[y * 2 + x - 5]);

} // TEST Array.copyFromTo