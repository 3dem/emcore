//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/object.h"
#include "em/base/array.h"
#include "em/image/image.h"

using namespace em;

TEST(Type, Basic) {
    // Test basic properties of Type singleton instances
    ConstTypePtr i = em::TypeInt;
    ASSERT_EQ(i->getName(), "int");
    ASSERT_TRUE(i->isPod());
    ASSERT_EQ(i->getSize(), sizeof(int));
    std::cout << std::endl << *i << std::endl;

    ConstTypePtr f = em::TypeFloat;
    ASSERT_EQ(f->getName(), "float");
    ASSERT_TRUE(f->isPod());
    ASSERT_EQ(f->getSize(), sizeof(float));
    std::cout << *f << std::endl;

    ConstTypePtr d = em::TypeDouble;
    ASSERT_EQ(d->getName(), "double");
    ASSERT_TRUE(d->isPod());
    ASSERT_EQ(d->getSize(), sizeof(double));
    std::cout << *d << std::endl;
}

TEST(Type, General) {
    ConstTypePtr f = em::TypeFloat;

    size_t n = 10;
    float * array = new float[n];
    for (size_t i = 0; i < n; i++)
    {
    array[i] = i * 10;
    }

    f->toStream(array, std::cout, n);
    std::cout << std::endl;

    float * array2 = new float[n/2];
    for (size_t i = 0; i < n/2; i++)
    {
    array2[i] = i;
    }

    // Test copy of some elements
    f->copy(array2, array, n/2);
    for (size_t i = 0; i < n/2; i++)
    {
    ASSERT_EQ(array[i], array2[i]);
    }
    f->toStream(array, std::cout, n);
    std::cout << std::endl;

    delete [] array;
    delete [] array2;

} // TEST(ArrayTest, ArrayDim)


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

    //std::cout << Av.toString() << std::endl;

    Array A2(A);
    ArrayView<int> Av2 = A2.getView<int>();
    //std::cout << Av2.toString() << std::endl;

    //std::cout << "Array printed: " << std::endl;
    //std::cout << A << std::endl;

    //Image
    Image img(ArrayDim(10, 10), em::TypeDouble);
    ObjectDict &header = img.getHeader();
    header["x"] = 10;
    header["y"] = 20.5;
    //header["filename"] = "/path/to/image/";
    std::cout << img << std::endl;

} // TEST(ArrayTest, Constructor)
