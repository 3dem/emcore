//
// Created by josem on 11/23/17.
//

#include <map>
#include <iostream>

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

    void readArgs(int argc, const char ** argv)
    {
        this->argc = argc;
        this->argv = argv;
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


int Program::start(int argc, const char **argv)
{
     auto args = docopt::docopt(getUsage(), { argv + 1, argv + argc },
                                true, getName());

    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    return run();
} // function start


