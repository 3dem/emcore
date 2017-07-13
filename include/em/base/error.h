//
// Created by josem on 7/13/17.
//

#ifndef EM_CORE_ERROR_H
#define EM_CORE_ERROR_H

#include <cstdlib>

#include <string>
#include <iostream>

namespace em {

    /** Class type for the errors thrown by the exceptions.
     */

    class Error {
    public:
        int errorCode; ///< Error code
        std::string msg; ///< Message of this error
        std::string filename; ///< Filename where the error was produced in the code
        long line; ///< Line number where the error was produced

        Error(const std::string &msg, const std::string &filename, const long line, const int errorCode=0);
    };

    /** Define how an Error is put into an stream.
     * (mainly for printing errors to the console)
     */
    std::ostream& operator<< (std::ostream &ostream, const Error &err);

}

// The following macro throws an Exception with a given error message
// and provides the file name and the line number
#define THROW_ERROR(msg) throw Error(msg, __FILE__, __LINE__)

// This other macro throws an Error using the errno macro from the system
#define THROW_SYS_ERROR(msg) throw Error(msg, __FILE__, __LINE__, errno)

#endif //EM_CORE_ERROR_H

