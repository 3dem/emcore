//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/base/object.h"
#include "em/base/array.h"
#include "em/image/image.h"



using namespace em;

TEST(Type, Basic) {
    // Test basic properties of Type singleton instances
    ConstTypePtr i = em::TypeInt;
    ASSERT_EQ(i->getName(), "int32");
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

    ConstTypePtr i8 = em::TypeInt8;
    ASSERT_EQ(i8->getName(), "int8");
    ASSERT_TRUE(i8->isPod());
    ASSERT_EQ(i8->getSize(), sizeof(int8_t));
    std::cout << *i8 << std::endl;

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

    std::cout << "sizeof int8_t: " << sizeof(int8_t) << std::endl;
    std::cout << "Signed Int8: " << *em::TypeInt8 << std::endl;
    std::cout << "Unsigned short: " <<  *em::TypeUShort << std::endl;
    std::cout << "Int: " <<  *em::TypeInt << std::endl;

} // TEST(ArrayTest, ArrayDim)


TEST(ArrayDim, Defaults) {

    // Default ctor of ArrayDim should set everything to 0
    ArrayDim adim;
    ASSERT_TRUE((adim == ArrayDim(0, 0, 0, 0)));

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

    ASSERT_TRUE(adim4 == adim3);
    ASSERT_EQ(adim4.getSize(), 100 * 100 * 100);
    ASSERT_EQ(adim4.getItemSize(), 100 * 100);


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

} // TEST(ArrayTest, Constructor)


TEST(Error, Basics) {

    std::string errorMsg = "Testing error";

    try
    {
        THROW_ERROR(errorMsg);
    }
    catch (Error &err)
    {
        ASSERT_EQ(err.msg, errorMsg);
        ASSERT_EQ(err.fileName, __FILE__);
        std::cout << err << std::endl;
    }

    try
    {
        FILE * pFile;
        pFile = fopen ("unexist.ent","r");
        if (pFile == NULL)
            THROW_SYS_ERROR("Error opening file unexist.ent");
    }
    catch (Error &err)
    {
        ASSERT_EQ(err.errorCode, ENOENT); // Not such file or directory error
        ASSERT_EQ(err.fileName, __FILE__);
        std::cout << err << std::endl;
    }
} // TEST(ArrayTest, Constructor)

