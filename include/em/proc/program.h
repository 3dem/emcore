//
// Created by Jose Miguel de la Rosa Trevin on 2017-11-23.
//

#ifndef EM_CORE_PROGRAM_H
#define EM_CORE_PROGRAM_H

#include <string>

namespace em
{
    class ProgramImpl;

    /**
     * Base class for all programs. It contains functionality for parsing
     * input arguments. This class provides access to input params and
     * handles their conversion.
     *
     */
    class Program
    {
    public:
        /** Default constructor */
        Program();

        /** Destructor */
        ~Program();

        /** Returns the name of the program */
        virtual std::string getName() const = 0;

        /** Define the usage of the program from where the valid input
         * arguments will be defined. (se docopt)
         */
        virtual std::string getUsage() const = 0;

        /** Public function used to start a program. */
        int start(int argc, const char ** argv);

        /** Return True if the argument 'arg' was provided. */
        bool checkArg(const std::string &arg) const;

        /** Return the value of the provide argument 'arg' */
        std::string getArg(const std::string & arg) const;

        /** Return the value of the provide argument 'arg' as integer */
        int getIntArg(const std::string &arg) const;

        /** Return the value of the provide argument 'arg' as a float */
        float getFloatArg(const std::string &arg) const;

    protected:
        /** Implement the job that this program does. */
        virtual int run() = 0;

        /** This function should be implemented for reading the values
         * from the input arguments. If an error is raised inside this function,
         * the error message will be printed, together with the usage string.
         */
        virtual void readArgs() = 0;

        /** Hold the implementation details of the Program class. */
        class Impl;
        Impl * impl;

    }; // class ImageProcessor

} // namespace em

#endif //EM_CORE_PROGRAM_H
