//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/object.h"
#include "em/base/array.h"
#include "em/image/image.h"

using namespace em;


TEST(ArrayDim, Defaults) {

    // Default ctor of ArrayDim should set everything to 0
    ArrayDim adim;
    ASSERT_TRUE((adim == ArrayDim(0, 0, 0, 0)));

    // When at least one of the dimensions is set
    // the rest will be set to 1
    ArrayDim adim2(100);
    ASSERT_TRUE((adim2 == ArrayDim(100, 1, 1, 1)));


} // TEST(ArrayTest, ArrayDim)


TEST(ArrayTest, Constructor) {

    ArrayDim adim(10, 10);
    Array A(adim, TypeInt);
    ArrayView<int> Av = A.getView<int>();

    Av.assign(11);
    Av(3, 3) = 20;
    Av(4, 4) = 20;
    Av(5, 5) = 20;
    int * ptr = Av.getDataPointer();
    ptr[10] = 15;

    std::cout << Av.toString() << std::endl;

    Array A2(A);
    ArrayView<int> Av2 = A2.getView<int>();
    std::cout << Av2.toString() << std::endl;

    //Image
    Image img;
    ObjectDict &header = img.getHeader();
    header["x"] = 10;
    header["y"] = 20.5;
    //header["filename"] = "/path/to/image/";

} // TEST(ArrayTest, Constructor)
