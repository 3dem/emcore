// A simple program

#include <iostream>
#include <math.h>

#include "em/base/error.h"
#include "em/image/image.h"
#include "em/proc/program.h"


using namespace em;

static const char USAGE[] =
        R"(em-image.

    Usage:
      em-image (<input> | create <xdim> [<ydim> [<zdim>]])
                       [(add|sub|mul|div) <file_or_value>   |
                        (shift|rotate) ((x|y|z) <value>)... |
                         rotate <value>                     |
                         flip (x|y|z)...                    |
                         scale  <scale_factor>              |
                         scale angpix <old> <new>           |
                         scale (x|y|z) <new_dim>            |
                        (lowpass|highpass) <freq>           |
                        (bandpass <low_freq> <high_freq>)
                       ]... <output>

    Options:
      <input>       An input file or a pattern matching many files.
      <output>      An output file or a suffix when many files are produced.
      -h --help     Show this screen.
      --version     Show version.
      --otype       Output file type
)";


class EmImageProgram: public Program
{
public:
    virtual std::string getName() const override
    {
        return "em-image";
    } // function getName

    virtual std::string getUsage() const override
    {
        return USAGE;
    }

    virtual StringVector getCommands() const override
    {
        return {"create", "add", "sub", "mul", "div", "shift", "rotate",
                "flip", "scale"};
    }

protected:
    virtual int run() override ;
    virtual void readArgs() override ;

private:
    std::string inputFn = "";
    std::string outputFn = "";
}; // class EmImageProgram

int EmImageProgram::run()
{
    std::cerr << "Doing nothing for now..." << std::endl;


    return 0;
} // function EmImageProgram.run

void EmImageProgram::readArgs()
{
    if (checkArg("<input>"))
    {
        inputFn = getArg("<input>");
        std::cout << "Input file: " << inputFn << std::endl;
    }
    if (checkArg("<output>"))
    {
        outputFn = getArg("<output>");
        std::cout << "Output file: " << outputFn << std::endl;
    }

    StringVector commands = ;
    // Store the pointers to each of the commands
    std::vector<size_t> cmdPos;
    size_t i = 0;
    auto& args = getArgs();

    for (auto &arg: args)
    {
        // Check if args is in commands
        for (auto &cmd: commands)
            if (arg == cmd)
            {
                cmdPos.push_back(i);
                break;
            }
        ++i;
    }

    std::cout << "Found commands: " << std::endl;
    for (auto& pos: cmdPos)
        std::cout << args[pos] << std::endl;

} // function EmImageProgram.readArgs


int main (int argc, const char **argv)
{
    EmImageProgram program;
    return program.start(argc, argv);
}
