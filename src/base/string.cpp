//
// Created by Jose Miguel de la Rosa Trevin on 2018-01-02.
//

#include <iostream>
#include <algorithm>

#include "em/base/error.h"
#include "em/base/string.h"

using namespace emcore;

const std::string String::SPACES = " \t\n\r";

std::string String::trim(const std::string &input)
{
    auto begin = input.find_first_not_of(SPACES);

    if (begin == std::string::npos)
        return "";

    auto end = input.find_last_not_of(SPACES);
    return input.substr(begin, end - begin + 1);
} // function String::trim

StringVector String::split(const char *str, const char sep)
{
    StringVector result;
    do
    {
        const char *begin = str;

        while(*str != sep && *str)
            ++str;

        if (str > begin)
            result.emplace_back(begin, str);
    } while (0 != *str++);

    return result;
} // function String::split (char*)

StringVector String::split(const std::string &input, const char sep)
{
    return split(input.c_str(), sep);
} // function String::split (string)

std::string String::join(StringVector input, const char sep)
{
    if (input.empty())
        return "";

    std::string result(input[0]);
    for (auto it = input.cbegin() + 1; it < input.cend(); ++it)
    {
        if (sep)
            result += sep;
        result += *it;
    }

    return result;
} // function String::join

std::string String::replace(const std::string &input,
                            const std::string &oldSubString,
                            const std::string &newSubString)
{
    size_t index = 0;
    size_t nOld = oldSubString.size();
    size_t nNew = newSubString.size();
    std::string output(input);

    while (true) {
        /* Locate the substring to replace. */
        index = output.find(oldSubString, index);
        if (index == std::string::npos)
            break;

        /* Make the replacement. */
        output.replace(index, nOld, newSubString);

        /* Advance index forward so the next iteration doesn't pick it up as well. */
        index += nNew;
    }

    return output;
} // function String::replace

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

// According to:
// https://stackoverflow.com/questions/313970/how-to-convert-stdstring-to-lower-case
// the following implementations of toUpper and toLower will only work for ASCII chars

std::string String::toUpper(const std::string &input)
{
    auto output = input;
    std::transform(output.begin(), output.end(), output.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return output;
} // String::toUpper

std::string String::toLower(const std::string &input)
{
    auto output = input;
    std::transform(output.begin(), output.end(), output.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return output;
} // String::toUpper