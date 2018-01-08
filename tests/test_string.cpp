//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/string.h"



using namespace em;


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
