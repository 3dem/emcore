//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/base/object.h"
#include "em/base/array.h"
#include "em/image/image.h"
#include "em/base/timer.h"


using namespace em;


TEST(Object, Basic)
{
    // Copy constructor
    em::Object o(1);
    int x = o;
    ASSERT_EQ(o.getType(), em::typeInt32);
    ASSERT_EQ(x, 1);
    o = 2;
    ASSERT_EQ(int(o), 2);

    em::Object o2(3.5); // Type should be double
    ASSERT_EQ(o2.getType(), em::typeDouble);
    o2 = 1.3f; // Now type should change to float
    ASSERT_EQ(o2.getType(), em::typeFloat);
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

    Image img(ArrayDim(10, 10), typeFloat);
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

TEST(Object, Parsing)
{
    // Test the parsing methods
    Object o1 = 0.0, o2 = 1.0;
    std::string pi("3.14159");
    std::stringstream ss(pi);
    o1.fromStream(ss);
    o2.fromString(pi);
    ASSERT_FLOAT_EQ((double)o1, 3.14159);
    ASSERT_FLOAT_EQ((double)o2, 3.14159);
    ASSERT_EQ(o1.toString(), pi);
    // FIXME: The following Object comparision throws an exception
    // ASSERT_FLOAT_EQ(o1, o2);
    std::cout << o1.toString() << std::endl;
} // TEST Object.Parsing