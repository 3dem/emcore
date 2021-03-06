//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "emc/base/string.h"
#include "emc/base/error.h"



using namespace emcore;


TEST(String, trim)
{
    std::string e;
    ASSERT_EQ(String::trim(e), e);
    ASSERT_EQ(String::trim(e + " \n\n\t\t   "), e);

    std::string s1(" abc ");
    std::string g1("abc");
    ASSERT_EQ(String::trim(g1), g1);
    ASSERT_EQ(String::trim(s1), g1);
    ASSERT_EQ(String::trim(s1 + "\n"), g1);

    std::string s2(" abc\t 123 xyz");
    std::string g2("abc\t 123 xyz");
    ASSERT_EQ(String::trim(s2), g2);
    ASSERT_EQ(String::trim(s2 + "\n456\n"), g2 + "\n456");

} // TEST(String, trim)

TEST(String, split)
{
    ASSERT_EQ(String::split("a b c"), StringVector({"a", "b", "c"}));
    ASSERT_EQ(String::split(" a  b   c  "), StringVector({"a", "b", "c"}));
    ASSERT_EQ(String::split(""), StringVector());
    ASSERT_EQ(String::split("   "), StringVector());
    ASSERT_EQ(String::split("123  456 789"), StringVector({"123", "456", "789"}));
    ASSERT_EQ(String::split("0.000000000   1.00"),
              StringVector({"0.000000000", "1.00"}));
} // TEST(String, trim)

TEST(String, join)
{
    ASSERT_EQ(String::join({}), "");
    ASSERT_EQ(String::join({"a"}), "a");
    ASSERT_EQ(String::join({"a", "b", "c"}), "abc");
    ASSERT_EQ(String::join({}, '/'), "");
    ASSERT_EQ(String::join({"a"}, '/'), "a");
    ASSERT_EQ(String::join({"a", "b", "c"}, '/'), "a/b/c");
} // TEST(String, trim)

TEST(String, toNumber)
{
    ASSERT_EQ(String::toInt("100"), 100);
    ASSERT_EQ(String::toInt("3"), 3);
    ASSERT_EQ(String::toInt("-100"), -100);
    ASSERT_EQ(String::toInt("-3"), -3);
    ASSERT_THROW(String::toInt("100F"), Error);
    ASSERT_THROW(String::toInt("50 50"), Error);
    ASSERT_THROW(String::toInt(""), Error);

    ASSERT_FLOAT_EQ(String::toFloat("100"), 100);
    ASSERT_FLOAT_EQ(String::toFloat("3.5"), 3.5);
    ASSERT_FLOAT_EQ(String::toFloat("-100.333"), -100.333);
    ASSERT_FLOAT_EQ(String::toFloat("-3.1e-2"), -3.1e-2);
    ASSERT_THROW(String::toFloat("100F"), Error);
    ASSERT_THROW(String::toFloat("50 50"), Error);
    ASSERT_THROW(String::toFloat(""), Error);

    ASSERT_FLOAT_EQ(String::toDouble("10000000.123456"), 10000000.123456);
    ASSERT_FLOAT_EQ(String::toDouble("3.5"), 3.5);
    ASSERT_FLOAT_EQ(String::toDouble("-100.333"), -100.333);
    ASSERT_FLOAT_EQ(String::toDouble("-3.1e-12"), -3.1e-12);
    ASSERT_THROW(String::toDouble("100F"), Error);
    ASSERT_THROW(String::toDouble("50 50"), Error);
    ASSERT_THROW(String::toDouble(""), Error);

    ASSERT_EQ(String::toLongInt("10000000123456"), 10000000123456);
    ASSERT_EQ(String::toLongInt("3"), 3);
    ASSERT_EQ(String::toLongInt("-100"), -100);
    ASSERT_EQ(String::toLongInt("-33333"), -33333);
    ASSERT_EQ(String::toDouble("-3e-12"), -3e-12);
    ASSERT_THROW(String::toLongInt("100F"), Error);
    ASSERT_THROW(String::toLongInt("50.50"), Error);
    ASSERT_THROW(String::toLongInt(""), Error);

} // TEST(String, toNumber)

TEST(String, replace)
{
    std::string test1 = "Replace aaa by bbb will have more bbb";

    ASSERT_EQ(String::replace(test1, "aaa", "bbb"),
              "Replace bbb by bbb will have more bbb");

    ASSERT_EQ(String::replace(test1, "bbb", "aaa"),
              "Replace aaa by aaa will have more aaa");

    ASSERT_EQ(String::replace(test1, "bbb", "b"),
              "Replace aaa by b will have more b");

    ASSERT_EQ(String::replace(test1, "bbb", ""),
              "Replace aaa by  will have more ");

    ASSERT_EQ(String::replace(test1, "ccc", "b"), test1);

} // TEST(String, trim)

TEST(String, toLowerUpper)
{
    ASSERT_EQ(String::toLower("ABC"), "abc");
    ASSERT_EQ(String::toLower("ABC123abc"), "abc123abc");
    ASSERT_EQ(String::toLower("aBcD"), "abcd");
    ASSERT_EQ(String::toLower("abcd"), "abcd");

    ASSERT_EQ(String::toUpper("ABC"), "ABC");
    ASSERT_EQ(String::toUpper("ABC123abc"), "ABC123ABC");
    ASSERT_EQ(String::toUpper("aBcD"), "ABCD");
    ASSERT_EQ(String::toUpper("abcd"), "ABCD");

} // TEST(String, toNumber)