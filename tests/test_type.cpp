//
// Created by Jose Miguel de la Rosa Trevin on 2018-02-01.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/base/object.h"
#include "em/base/array.h"
#include "em/base/image.h"
#include "em/base/timer.h"
#include "em/base/container_priv.h"



using namespace em;


TEST(Type, Basic) {
    // Test basic properties of Type singleton instances
    auto i = typeInt32;
    auto ii = Type::get<uint32_t>();
    auto iii = ii;
    ASSERT_EQ(i.getName(), "int32");
    ASSERT_TRUE(i.isPod());
    ASSERT_EQ(i.getSize(), sizeof(int));
    std::cout << std::endl << i << std::endl;

    auto f = typeFloat;
    ASSERT_EQ(f.getName(), "float");
    ASSERT_TRUE(f.isPod());
    ASSERT_EQ(f.getSize(), sizeof(float));
    std::cout << f << std::endl;

    auto d = typeDouble;
    ASSERT_EQ(d.getName(), "double");
    ASSERT_TRUE(d.isPod());
    ASSERT_EQ(d.getSize(), sizeof(double));
    std::cout << d << std::endl;

    auto i8 = typeInt8;
    ASSERT_EQ(i8.getName(), "int8");
    ASSERT_TRUE(i8.isPod());
    ASSERT_EQ(i8.getSize(), sizeof(int8_t));
    std::cout << i8 << std::endl;

    std::cout << em::Type::get<size_t>() << std::endl;
    std::cout << em::Type::get<uint64_t>() << std::endl;
}

TEST(Type, General) {
    size_t n = 10;
    float * array = new float[n];
    for (size_t i = 0; i < n; i++)
        array[i] = i * 10;

    typeFloat.toStream(array, std::cout, n);
    std::cout << std::endl;

    float * array2 = new float[n/2];
    for (size_t i = 0; i < n/2; i++)
        array2[i] = i;

    // Test copy of some elements
    typeFloat.copy(array2, array, n/2);
    for (size_t i = 0; i < n/2; i++)
        ASSERT_EQ(array[i], array2[i]);

    std::cout << "Float array: " << std::endl;
    typeFloat.toStream(array, std::cout, n);
    std::cout << std::endl;

    // Test type casting
    int32_t * arrayInt = new int32_t[n];
    typeInt32.operate(Type::CAST, array, typeFloat, arrayInt, n);

    for (size_t i = 0; i < n; ++i)
        ASSERT_FLOAT_EQ(array[i], (float)arrayInt[i]);

    std::cout << "Int32 array: " << std::endl;
    typeInt32.toStream(arrayInt, std::cout, n);
    std::cout << std::endl;

    int32_t * arrayInt2 = new int32_t[n];
    std::string arrayValues("0 1 2 3 4 50 60 70 80 90 123.45 123.45 123.45");
    std::stringstream ss(arrayValues);
    typeInt32.fromStream(ss, arrayInt2, n);

    double refValue = 123.45;
    double d1 = 0.0;
    Object o1 = 0.0;
    // Parse from stream into a single double
    typeDouble.fromStream(ss, &d1, 1);
    ASSERT_FLOAT_EQ(refValue, d1);
    // Parse from stream into an Object (initialized to double)
    typeDouble.fromStream(ss, o1.getData(), 1);
    ASSERT_FLOAT_EQ(refValue, o1.get<double>());

    for (size_t i = 0; i < n; i++)
        ASSERT_EQ(arrayInt[i], arrayInt2[i]);

    delete [] array;
    delete [] array2;
    delete [] arrayInt;
    delete [] arrayInt2;

    std::cout << "sizeof int8_t: " << sizeof(int8_t) << std::endl;
    std::cout << "Signed Int8: " << typeInt8 << std::endl;
    std::cout << "Unsigned short: " <<  typeUInt16 << std::endl;
    std::cout << "Int: " <<  typeInt32 << std::endl;

    ASSERT_EQ(Type::inferFromString("100"), typeInt32);
    ASSERT_EQ(Type::inferFromString("100.00"), typeFloat);
    ASSERT_EQ(Type::inferFromString("192.168.100.1"), typeString);
    ASSERT_EQ(Type::inferFromString("/this/is/a/path"), typeString);
    std::string line("100 3.4 4.5 abc");
    const char * ptr = line.data();
    ASSERT_EQ(Type::inferFromString(line), typeString);
    ASSERT_EQ(Type::inferFromString(ptr, 3), typeInt32);
    ASSERT_EQ(Type::inferFromString(ptr+4, 3), typeFloat);
    ASSERT_EQ(Type::inferFromString(ptr+8, 3), typeFloat);
    ASSERT_EQ(Type::inferFromString(ptr+12, 3), typeString);

} // TEST(Type, General)


TEST(Type, Indexing)
{
    Type t1 = typeInt8;
    Type t2 = typeInt8;


//
//    // Test use of Type as index of maps
//    TypeVector tv = {typeInt8, typeInt16, typeInt32};
//    StringVector sv = {"type-int8", "type-int16", "type-int32"};
//    TypeStringMap map;
//
//    for (int i = 0; i < tv.size(); ++i)
//        map[tv[i]] = sv[i];
//
//    TypeVector tv2 = {typeInt8, typeInt16, typeInt32};
//    for (int i = 0; i < tv2.size(); ++i)
//        ASSERT_NE(map.find(tv2[i]), map.end());

} // TEST Type.Indexing

TEST(TypeContainer, Basic)
{
    // Empty container
    TypedContainer gc;
    ASSERT_TRUE(gc.getType().isNull());
    ASSERT_EQ(gc.getType(), typeNull);
    ASSERT_EQ(gc.getData(), nullptr);

    // Let's check memory is not deleted after container dtor
    size_t n = 10;
    int *array = new int[n];
    for (size_t i = 0; i < n; ++i)
        array[i] = i;

    {
        auto& t = Type::get<int>();
        TypedContainer gc2(t, n, array);
        TypedContainer gc3(t, n); // should allocate its memory
        int * array2 = static_cast<int*>(gc3.getData());
        t.copy(gc2.getData(), gc3.getData(), n);
        for (size_t i = 0; i < n; ++i)
            ASSERT_EQ(array2[i], i);
    } // Here gc2 should be destroyed, but not the memory

    for (size_t i = 0; i < n; ++i)
        ASSERT_EQ(array[i], i);

} // TEST Container.Basic


