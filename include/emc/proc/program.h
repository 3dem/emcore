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
        /** Class to facilitate the access to commands passed as input
         * and their respective arguments.
         */
        class Command
        {
        public:
            /** Private constructor, Only programs can create Command instances */
            Command(Program& program, const std::string name, int index);

            /** Return the name of the Command */
            std::string getName() const;

            /** Return the value of the argument with that name */
            std::string getArg(const std::string &argName) const;

            /** Return the value of the argument with that name as integer */
            int getArgAsInt(const std::string &argName) const;

            /** Return the value of the argument with that name as float */
            float getArgAsFloat(const std::string &argName) const;

        private:
            Program &program;
            std::string name;
            int index;  // the index of this command

        friend class Program;
        }; // class Command

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
        int main(int argc, const char ** argv);

        /** Return the value of the provide argument 'arg' */
        const Command& getCommand(const std::string & arg) const;

        /** Return the list of all arguments */
        const std::vector<Command>& getCommandList() const;

        /** Return True if the argument 'arg' was provided. */
        bool hasArg(const std::string &arg) const;

        /** Return the value of the argument with that name */
        std::string getArg(const std::string &argName) const;

        /** Return the value of the argument with that name as integer */
        int getArgAsInt(const std::string &argName) const;

        /** Return the value of the argument with that name as float */
        float getArgAsFloat(const std::string &argName) const;

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
