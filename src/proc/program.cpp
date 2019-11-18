//
// Created by josem on 11/23/17.
//

#include <map>
#include <iostream>
#include <cstring>

#include "docopt.h"

#include "emc/base/type.h"
#include "emc/proc/program.h"


using namespace emcore;

using DocoptArgs = std::map<std::string, docopt::value>;


// ----------------------Program::Impl Implementation -------------------------

class Program::Impl
{
public:
    std::map<std::string, docopt::value> docoptArgs;
    // Store commands and options recognized by this program
    std::vector<Program::Command> commands;

    /** Create the command list from the input args */
    void createCommands(Program &program, int argc, const char **argv)
    {
        // Will store the word and count.
        std::map<std::string, int> cmdCount;

        const char ** iter = argv + 1;
        int n = argc - 1;  // remove the program name

        //for(auto const& arg : docoptArgs)
        for (int i = 0; i < n; ++i)
        {
            // Uncomment the following line for debugging
            //auto &argName = arg.first;
            const char * argName = *iter;
            ++iter;

            if (docoptArgs.find(argName) == docoptArgs.end())
                continue;

            std::cout << argName << ": " << docoptArgs[argName] << std::endl;

            // Check if it is command (i.e not starting with < or -)
            char c = argName[0];

            if (c == '<' or c == '-' or !program.hasArg(argName))
                continue;

            //Look if it's already there.
            if (cmdCount.find(argName) == cmdCount.end())
                cmdCount[argName] = 0;
            else // Then we've already seen it before..
                cmdCount[argName]++; // Just increment it.

            commands.push_back(Command(program, argName, cmdCount[argName]));
        }
    }

}; // class ProgramImpl


// ----------------------Program::Command  Implementation --------------------

Program::Command::Command(Program& program, const std::string name, int index):
        program(program), name(name), index(index)
{}

std::string Program::Command::getName() const
{
    return name;
}

std::string Program::Command::getArg(const std::string &argName) const
{
    auto &args = program.impl->docoptArgs;

    ASSERT_ERROR((args.find(argName) == args.end()),
                 std::string("Argument ") + argName + std::string("not found.\n"))

    return program.impl->docoptArgs[argName].asStringList()[index];
}

int Program::Command::getArgAsInt(const std::string &argName) const
{
    return String::toInt(getArg(argName).c_str());
}

float Program::Command::getArgAsFloat(const std::string &argName) const
{
    return String::toFloat(getArg(argName).c_str());
}

// ----------------------Program Implementation -------------------------------

Program::Program()
{
    impl = new Impl();
} // Ctor

Program::~Program()
{
    delete impl;
} // Dtor

bool Program::hasArg(const std::string &argName) const
{
    if (impl->docoptArgs.find(argName) == impl->docoptArgs.end())
        return false;
    const auto &darg = impl->docoptArgs[argName];
    if (darg.isBool())
        return darg.asBool();
    if (darg.isString())
        return  !darg.asString().empty();
    if (darg.isStringList())
        return !darg.asStringList().empty();
    if (darg.isLong())
        return darg.asLong() > 0;

    return bool(darg);
} //function Program.hasArg

const std::vector<Program::Command>& Program::getCommandList() const
{
    return impl->commands;
}

std::string Program::getArg(const std::string &argName) const
{
    return impl->docoptArgs[argName].asString();
}

int Program::getArgAsInt(const std::string &argName) const
{
    return String::toInt(getArg(argName).c_str());
}

float Program::getArgAsFloat(const std::string &argName) const
{
    return String::toFloat(getArg(argName).c_str());
}

int Program::main(int argc, const char **argv)
{
    try
    {
        std::cout << EM_CORE_VERSION
                  << " (" << EM_CORE_TIMESTAMP << ")"
                  << std::endl << std::endl;

        // impl->readArgs(argc, argv, getCommands());
        impl->docoptArgs = docopt::docopt(getUsage(),
                                          {argv + 1, argv + argc},
                                          true, getName());

        impl->createCommands(*this, argc, argv);

        for (auto const &cmd: impl->commands)
            std::cout << "DEBUG: cmd: " << cmd.getName() << std::endl;

        readArgs();
        return run();
    }
    catch (Error &e)
    {
        std::cerr << e << std::endl;
        return -1;
    }
} // function start


