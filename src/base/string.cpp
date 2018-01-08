//
// Created by Jose Miguel de la Rosa Trevin on 2018-01-02.
//

#include <iostream>
#include "em/base/string.h"

using namespace em;

const std::string String::SPACES = " \t\n\r";

std::string String::trim(const std::string &input)
{
    auto begin = input.find_first_not_of(SPACES);

    if (begin == std::string::npos)
        return "";

    auto end = input.find_last_not_of(SPACES);
    return input.substr(begin, end - begin + 1);
} // function Sring::trim