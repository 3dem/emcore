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

protected:
    virtual int run() override ;
    virtual void readArgs() override ;
}; // class EmImageProgram

int EmImageProgram::run()
{
    std::cerr << "Doing nothing for now..." << std::endl;

    return 0;
} // function EmImageProgram.run

void EmImageProgram::readArgs()
{

} // function EmImageProgram.readArgs


int main (int argc, const char **argv)
{
    EmImageProgram program;
    return program.start(argc, argv);
}
