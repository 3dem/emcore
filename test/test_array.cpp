//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/image/image.h"


using namespace em;


TEST(Array, Basic)
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
            ASSERT_EQ(data[i], avAll(x, y, z, N));
            ASSERT_EQ(data[i], N);
        }
    }
    catch (Error &e)
    {
        std::cerr << e << std::endl;
    }


} // TEST(Array, getAlias)