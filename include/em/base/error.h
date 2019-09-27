//
// Created by josem on 7/13/17.
//

#ifndef EM_CORE_ERROR_H
#define EM_CORE_ERROR_H

#include <cstdlib>
#include <string>
#include <iostream>


namespace emcore {

    /** Class type for the errors thrown by the exceptions.
     */

    class Error: public std::exception {
    public:
        int errorCode; ///< Error code
        std::string msg; ///< Message of this error
        std::string fileName; ///< Filename where the error was produced in the code
        std::string functionName; ///< Name of the function where the error was produced
        long line; ///< Line number where the error was produced

        Error(const std::string &msg, const std::string &filename, const long line,
              const int errorCode=0, const std::string &func="");

        /** String representation of the Error */
        std::string toString() const;

        virtual const char* what() const throw() override ;
    };

    /** Define how an Error is put into an stream.
     * (mainly for printing errors to the console)
     */
    std::ostream& operator<< (std::ostream &ostream, const Error &err);

} // namespace emcore

// The following macro throws an Exception with a given error message
// and provides the file name and the line number
#define THROW_ERROR(msg) throw Error(msg, __FILE__, __LINE__, 0, __func__)

// This other macro throws an Error using the errno macro from the system
#define THROW_SYS_ERROR(msg) throw Error(msg, __FILE__, __LINE__, errno, __func__)

// Similar to THROW_ERROR but taking a condition as input and
// if the condition is evaluated to False, then an error is thrown
#define ASSERT_ERROR(cond, msg) if ((cond)) THROW_ERROR(msg);

#endif //EM_CORE_ERROR_H

