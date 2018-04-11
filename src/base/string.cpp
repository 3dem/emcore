//
// Created by Jose Miguel de la Rosa Trevin on 2018-01-02.
//

#include <iostream>

#include "em/base/error.h"
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
} // function String::trim

// Define a convenience macro to use sscanf,
// str and retval should be defined
#define SCAN_NUMBER(type, format) type retval; char dummy; \
if (sscanf(str, format, &retval, &dummy) == 1) return retval; \
THROW_ERROR("Type conversion error")

int String::toInt(const char *str)
{
    SCAN_NUMBER(int, "%d%c");
} // function String::toInt

float String::toFloat(const char *str)
{
    SCAN_NUMBER(float, "%f%c");
} // function String::toFloat

double String::toDouble(const char *str)
{
    SCAN_NUMBER(double, "%lf%c");
} // function String::toDouble

long long int String::toLongInt(const char *str)
{
    SCAN_NUMBER(long long int, "%lld%c");
} // function String::toLongInt

