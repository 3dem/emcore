//
// Created by Jose Miguel de la Rosa Trevin on 2018-01-02.
//

#ifndef EM_CORE_STRING_H
#define EM_CORE_STRING_H

#include <string>
#include <vector>

namespace em
{
    using StringVector = std::vector<std::string>;

    class String
    {
    public:
        // String constant with blank characters
        static const std::string SPACES; // " \t\n\r";

        /** Remove blank characters (spaces, tabs and newlines) from
         * the beginning and the end of the string.
         * @param input Input string
         * @return Input string without leading and trailing spaces
         */
        static std::string trim(const std::string &input);

        /** Split the string into tokens */
        static StringVector split(const char *str, const char sep = ' ');

        /** Convert a string to integer */
        static int toInt(const char *str);

        /** Convert a string to float */
        static float toFloat(const char *str);

        /** Convert a string to double */
        static double toDouble(const char *str);

        /** Convert a string to a long int */
        static long long int toLongInt(const char *str);

    }; // class String



} // namespace em

#endif //EM_CORE_STRING_H
