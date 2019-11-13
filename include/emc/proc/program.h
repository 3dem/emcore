//
// Created by Jose Miguel de la Rosa Trevin on 2017-11-23.
//

#ifndef EM_CORE_PROGRAM_H
#define EM_CORE_PROGRAM_H

#include <string>
#include "emc/base/string.h"


namespace emcore
{
    /**
     * Base class for all programs. It contains functionality for parsing
     * input arguments. This class provides access to input params and
     * handles their conversion.
     *
     */
    class Program
    {
    public:
        /** Argument class to manage the arguments values passed to the
         */
        class Argument
        {
        public:
            Argument(int argc, const char **argv);
            /** Return the name of the argument */
            std::string toString() const;

            /** Return the number of values of this argument (including 0,
             * the name of the argument.
             */
             size_t getSize() const;

            /** Return the value of the argument in a given position */
            const char * get(size_t pos = 1) const;

            /** Return the value of the argument as an int */
            int  getInt(size_t pos = -1) const;

            /** Return the value of the argument as a float */
            float getFloat(size_t pos = 1) const;

            /** Return the value of the argument as a String */
            std::string getString(size_t pos=1) const;

            /** Return the value of the argument in a given position
             * but casted to a given type.
             */
            template <class T>
            T get(size_t pos = 1) const;
        private:
            int argc;
            const char ** argv;
        }; // class Argument

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

        /** Return which are the command that this program recognizes.
         * NOTE: In the future this function could be removed and the
         * command can be inferred from the usage if we use a proper
         * language definition for the arguments.
         */
        virtual StringVector getCommands() const = 0;

        /** Public function used to start a program. */
        int main(int argc, const char ** argv);

        /** Return True if the argument 'arg' was provided. */
        bool hasArg(const std::string &arg) const;

        /** Return the value of the provide argument 'arg' */
        const Argument& getArg(const std::string & arg) const;

        /** Return the list of all arguments */
        const std::vector<Argument>& getArgList() const;

        /** Directly query values of parameters that are not commands */
        const std::string getValue(const char * arg) const;

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

} // namespace emcore

#endif //EM_CORE_PROGRAM_H
