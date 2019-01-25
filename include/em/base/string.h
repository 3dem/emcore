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
        static StringVector split(const std::string &input, const char sep = ' ');

        /** Join many input strings into one, using a separator character */
        static std::string join(StringVector input, const char sep = 0);

        /** Replace all occurrences of a substring and return a new string. */
        static std::string replace(const std::string& input,
                                   const std::string& oldSubString,
                                   const std::string& newSubString);

        /** Convert a string to integer */
        static int toInt(const char *str);

        /** Convert a string to float */
        static float toFloat(const char *str);

        /** Convert a string to double */
        static double toDouble(const char *str);

        /** Convert a string to a long int */
        static long long int toLongInt(const char *str);

        /** Convert a string to upper and lower case */
        static std::string toUpper(const std::string &input);
        static std::string toLower(const std::string &input);

    }; // class String



} // namespace em

#endif //EM_CORE_STRING_H
