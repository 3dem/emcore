//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/image/image.h"
#include "em/base/legacy.h"


using namespace em;

TEST(ArrayDim, Defaults) {

    // Default ctor of ArrayDim should set everything to 0
    ArrayDim adim;
    ASSERT_TRUE((adim == ArrayDim(0, 1, 1, 1)));

    // When at least one of the dimensions is set
    // the rest will be set to 1
    ArrayDim adim2(100);
    ASSERT_TRUE((adim2 == ArrayDim(100, 1, 1, 1)));
    ASSERT_EQ(adim2.getRank(), 1);

    std::cout << adim2 << std::endl;

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
    ArrayView<int> Av = A.getView<int>();

    // Check the assignment through the ArrayView class
    Av.assign(11);
    int * ptr = Av.getData();

    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(11, ptr[i]);


    // Now test the assignment through the Array class directly
    A = 22;
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

    //std::cout << Av.toString() << std::endl;

    Array A2(A);
    ArrayView<int> Av2 = A2.getView<int>();
    const int * data2 = Av2.getData();
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data2[i], ptr[i]);

    Array A3(adim, typeFloat);
    A3 = A;
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
            auto arraySingle = array.getAlias(i);
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


} // TEST(Array, getAlias)

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
            auto arraySingle = array.getAlias(i);
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


} // TEST(Array, getAlias)