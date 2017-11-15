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
    ConstTypePtr i = em::TypeInt32;
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
    size_t n = 10;
    float * array = new float[n];
    for (size_t i = 0; i < n; i++)
        array[i] = i * 10;

    TypeFloat->toStream(array, std::cout, n);
    std::cout << std::endl;

    float * array2 = new float[n/2];
    for (size_t i = 0; i < n/2; i++)
        array2[i] = i;

    // Test copy of some elements
    TypeFloat->copy(array2, array, n/2);
    for (size_t i = 0; i < n/2; i++)
        ASSERT_EQ(array[i], array2[i]);

    std::cout << "Float array: " << std::endl;
    TypeFloat->toStream(array, std::cout, n);
    std::cout << std::endl;

    // Test type casting
    int32_t * arrayInt = new int32_t[n];
    TypeInt32->cast(array, arrayInt, n, TypeFloat);

    for (size_t i = 0; i < n; ++i)
        ASSERT_FLOAT_EQ(array[i], (float)arrayInt[i]);

    std::cout << "Int32 array: " << std::endl;
    TypeInt32->toStream(arrayInt, std::cout, n);
    std::cout << std::endl;

    delete [] array;
    delete [] array2;
    delete [] arrayInt;

    std::cout << "sizeof int8_t: " << sizeof(int8_t) << std::endl;
    std::cout << "Signed Int8: " << *em::TypeInt8 << std::endl;
    std::cout << "Unsigned short: " <<  *em::TypeUInt16 << std::endl;
    std::cout << "Int: " <<  *em::TypeInt32 << std::endl;

} // TEST(Type, General)


TEST(Object, Basic)
{
    // Copy constructor
    em::Object o(1);
    int x = o;
    ASSERT_EQ(o.getType(), em::TypeInt32);
    ASSERT_EQ(x, 1);
    o = 2;
    ASSERT_EQ(int(o), 2);

    em::Object o2(3.5); // Type should be double
    ASSERT_EQ(o2.getType(), em::TypeDouble);
    o2 = 1.3f; // Now type should change to float
    ASSERT_EQ(o2.getType(), em::TypeFloat);
    float f = o2;
    ASSERT_FLOAT_EQ(f, 1.3f);

    o2 = 5.6f;

    float d, d2 = 5.6 + float(o2);
    float d3;

    size_t N = 100;
    float values [] = {1.5f, 2.3f, 5.7f, 3.2f, 10.f, 56.f};
    std::vector<Object> vobj;

    for (int i = 0; i < N; i++)
    {
        d = values[i % 6];
        vobj.push_back(Object(d));
    }

    for (int i = 0; i < N; ++i)
    {
        d = vobj[i];
        ASSERT_FLOAT_EQ(d, values[i % 6]);
    }

    const char * str = "This is a test string";

    Object o3;
    o3 = std::string(str);
    std::string s2 = o3;
    ASSERT_EQ(s2, str);

    Image img(ArrayDim(10, 10), TypeFloat);
    o3 = img;
    ASSERT_EQ(o3.getType(), Type::get<Image>());
    Image img2 = o3;

    // Test copy and equality
    o3 = std::string("One string");
    o2 = o3;
    ASSERT_EQ(o2, o3);

    Object o4(o3);
    ASSERT_EQ(o4, o3);

    ObjectVector ov;
    ov.resize(3);

    ov[0] = o3;
    ASSERT_EQ(ov[0], o3);
    ASSERT_EQ(ov[0], o2);
    //ASSERT_EQ((std::string) o2, (std::string) o3);
    Object o5(o2);
    ASSERT_EQ(o2, o5);

} // TEST Object.Basic


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

