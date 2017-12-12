//
// Created by josem on 11/23/17.
//

#include <map>
#include <iostream>
#include <em/base/type.h>

#include "docopt.h"
#include "em/proc/program.h"

using namespace em;

using DocoptArgs = std::map<std::string, docopt::value>;


class Program::Impl
{
public:
    std::map<std::string, docopt::value> docoptArgs;
    int argc;
    const char ** argv;
    // Store arguments as std::string without the program name
    StringVector args;

    void readArgs(int argc, const char ** argv)
    {
        this->argc = argc;
        this->argv = argv;
        this->args = {argv + 1, argv + argc};
    }
}; // class ProgramImpl


Program::Program()
{
    impl = new Impl();
} // Ctor

Program::~Program()
{
    delete impl;
} // Dtor

const StringVector& Program::getArgs() const
{
    return impl->args;
}

bool Program::checkArg(const std::string &arg) const
{
    return impl->docoptArgs.find(arg) != impl->docoptArgs.end();
} //function Program.checkArg

std::string Program::getArg(const std::string &arg) const
{
    return impl->docoptArgs[arg].asString();
}

int Program::start(int argc, const char **argv)
{
    std::cout << EM_CORE_VERSION
              << " (" << EM_CORE_TIMESTAMP << ")"
              << std::endl << std::endl;

    impl->readArgs(argc, argv);
    impl->docoptArgs = docopt::docopt(getUsage(), getArgs(),
                                true, getName());

    for(auto const& arg : impl->docoptArgs) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    std::cout << std::endl << std::endl;

    readArgs();
    return run();
} // function start


