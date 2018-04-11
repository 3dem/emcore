// A simple program

#include <iostream>
#include <math.h>

#include "em/base/error.h"
#include "em/image/image.h"
#include "em/proc/program.h"
#include "em/proc/operator.h"


using namespace em;

static const char USAGE[] =
        R"(em-image.

    Usage:
      em-image (<input> | create <xdim> [<ydim> [<zdim>]])
                       [(add|sub|mul|div) <file_or_value>   |
                        (shift|rotate) (x|y|z) <value>      |
                         rotate <value>                     |
                         flip (x|y|z)                       |
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

    void processCommand(const StringVector& args,
                        size_t startIndex, size_t endIndex);
}; // class EmImageProgram


// ---------------------- Implementation -------------------------------
int EmImageProgram::run()
{
    std::cerr << "Doing nothing for now..." << std::endl;


    return 0;
} // function EmImageProgram.run

void EmImageProgram::readArgs()
{
    if (hasArg("<input>"))
    {
        inputFn = getValue("<input>");
        std::cout << "Input file: " << inputFn << std::endl;
    }
    if (hasArg("<output>"))
    {
        outputFn = getValue("<output>");
        std::cout << "Output file: " << outputFn << std::endl;
    }

    auto& args = getArgList();

    std::cout << "Number of parsed arguments: " << args.size() << std::endl;

    std::string cmdName;

    for (auto& a: args)
    {
        cmdName = a.toString();

        if (cmdName == "add")
        std::cout << "argument: " << a.toString() << std::endl;
        std::cout << "   values: ";
        for (int i = 1; i < a.getSize(); ++i)
            std::cout << a.get(i) << " " << std::endl;
    }

} // function EmImageProgram.readArgs

ImageProcessor* getProcessorFromArg(const Program::Argument& arg)
{
    std::string cmdName = arg.toString();
    char op = 0;

    // Check first if the argument is for a basic arithmetic operation
    if (cmdName == "add")
        op = Type::ADD;
    else if (cmdName == "sub")
        op = Type::SUB;
    else if (cmdName == "mul")
        op = Type::MUL;
    else if (cmdName == "div")
        op = Type::DIV;

    if (op != 0)
    {
        return new ImageMathProc();
    }
}


int main (int argc, const char **argv)
{
    return EmImageProgram().main(argc, argv);
}
