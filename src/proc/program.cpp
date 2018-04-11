//
// Created by josem on 11/23/17.
//

#include <map>
#include <iostream>
#include <cstring>

#include "docopt.h"

#include <em/base/type.h>
#include "em/proc/program.h"

using namespace em;

using DocoptArgs = std::map<std::string, docopt::value>;


// ----------------------Program::Impl Implementation -------------------------

class Program::Impl
{
public:
    std::map<std::string, docopt::value> docoptArgs;
    int argc;
    const char ** argv;
    // Store commands and options recognized by this program
    std::vector<Argument> arguments;

    void readArgs(int argc, const char ** argv, const StringVector& commands)
    {
        this->argc = argc;
        this->argv = argv;

        // FIXME: This is a quick and dirty parsing of the commands
        // I don't want to spend more time right now to a proper parsing
        // We are assuming now that the program will receive a list of
        // commands or options and in between there are the arguments
        // of each command/option
        const char ** iter = argv + 1;
        int prevPos = -1;
        int n = argc - 1;  // remove the program name

        for (int i = 0; i < n; ++i)
        {
            // Check if the command is present
            const char * str = *iter;

            bool hasCmd = false;
            for (auto& cmd: commands)
                if (strcmp(*iter, cmd.c_str()) == 0)
                {
                    hasCmd = true;
                    break;
                }

            if (hasCmd)
            {
                if (prevPos > 0)
                    arguments.emplace_back(Argument(i - prevPos,
                                                    argv + 1 + prevPos));
                prevPos = i;
            }
            ++iter;
        }
        // Add the last command
        // FIXME: This now will also take the <output> or last positional
        // arguments as part of the last command...but it will not hurt for now
        if (prevPos > 0)
            arguments.emplace_back(Argument(n - prevPos, argv + 1 + prevPos));
    }

}; // class ProgramImpl


// ----------------------Program::Argument  Implementation --------------------

Program::Argument::Argument(int argc, const char **argv):argc(argc), argv(argv)
{}

const char* Program::Argument::get(size_t pos) const
{
    ASSERT_ERROR(pos > argc,
                 "Position is greater than the number of argument values");
    return argv[pos];
}

std::string Program::Argument::toString() const
{
    return argv[0];
} // function Program::Argument.toString

size_t Program::Argument::getSize() const
{
    return argc;
} // function Program::Argument.getSize


// ----------------------Program Implementation -------------------------------

Program::Program()
{
    impl = new Impl();
} // Ctor

Program::~Program()
{
    delete impl;
} // Dtor

bool Program::hasArg(const std::string &arg) const
{
    return impl->docoptArgs.find(arg) != impl->docoptArgs.end();
} //function Program.hasArg

const Program::Argument& Program::getArg(const std::string &arg) const
{
    THROW_ERROR("NOT IMPLEMENTED. ");
} // function Program.getArg

const std::vector<Program::Argument>& Program::getArgList() const
{
    return impl->arguments;
} // function Program.getArgList

const std::string Program::getValue(const char *arg) const
{
    return impl->docoptArgs[arg].asString();
}

int Program::main(int argc, const char **argv)
{
    std::cout << EM_CORE_VERSION
              << " (" << EM_CORE_TIMESTAMP << ")"
              << std::endl;

    impl->readArgs(argc, argv, getCommands());
    impl->docoptArgs = docopt::docopt(getUsage(),
                                      {argv + 1, argv + argc},
                                      true, getName());

//    for(auto const& arg : impl->docoptArgs) {
//        std::cout << arg.first << ": " << arg.second << std::endl;
//    }

    std::cout << std::endl << std::endl;

    readArgs();
    return run();
} // function start


