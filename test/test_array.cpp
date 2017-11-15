//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/image/image.h"


using namespace em;

TEST(ArrayDim, Defaults) {

    // Default ctor of ArrayDim should set everything to 0
    ArrayDim adim;
    ASSERT_TRUE((adim == ArrayDim(0, 1, 1, 1)));

    // When at least one of the dimensions is set
    // the rest will be set to 1
    ArrayDim adim2(100);
    ASSERT_TRUE((adim2 == ArrayDim(100, 1, 1, 1)));

    std::cout << adim2 << std::endl;

    // Test helper functions to compute the size
    ArrayDim adim3(100, 100, 1, 100);

    ASSERT_EQ(adim3.getSize(), 100 * 100 * 100);
    ASSERT_EQ(adim3.getItemSize(), 100 * 100);

    // Test copy constructor
    ArrayDim adim4(adim3);

    ASSERT_EQ(adim4, adim3);
    ASSERT_EQ(adim4.getSize(), 100 * 100 * 100);
    ASSERT_EQ(adim4.getItemSize(), 100 * 100);


} // TEST(ArrayTest, ArrayDim)


TEST(Array, Basic) {

    ArrayDim adim(10, 10);
    Array A(adim, TypeInt32);
    ArrayView<int> Av = A.getView<int>();

    Av.assign(11);
    Av(3, 3) = 20;
    Av(4, 4) = 20;
    Av(5, 5) = 20;
    int * ptr = Av.getDataPointer();
    ptr[10] = 15;

    //std::cout << Av.toString() << std::endl;

    Array A2(A);
    ArrayView<int> Av2 = A2.getView<int>();
    const int * data2 = Av2.getDataPointer();
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data2[i], ptr[i]);

    Array A3(adim, TypeFloat);
    A3.copy(A);
    ASSERT_EQ(A3.getType(), TypeFloat);
    auto data3f = static_cast<const float *>(A3.getDataPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3f[i], (float)ptr[i]);

    A3.copy(A, TypeUInt32);
    ASSERT_EQ(A3.getType(), TypeUInt32);
    auto data3ui = static_cast<const uint32_t *>(A3.getDataPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_EQ(data3ui[i], (uint32_t)ptr[i]);

    Array A4;
    A4.copy(A);
    ASSERT_EQ(A4.getType(), TypeInt32);
    auto data3i = static_cast<const int32_t *>(A3.getDataPointer());
    for (size_t i = 0; i < adim.getSize(); ++i)
        ASSERT_FLOAT_EQ(data3i[i], (int32_t)ptr[i]);
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

    Array array(adim, TypeFloat);

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

        float * data = avAll.getDataPointer();

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