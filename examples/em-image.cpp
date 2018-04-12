// A simple program

#include <iostream>
#include <iomanip>
#include <math.h>

#include "em/base/error.h"
#include "em/os/filesystem.h"
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
    ImagePipeProc pipeProc;
    StringVector inputList;

    void processCommand(const StringVector& args,
                        size_t startIndex, size_t endIndex);

    ImageProcessor* getProcessorFromArg(const Program::Argument& arg);
}; // class EmImageProgram


// ---------------------- Implementation -------------------------------
int EmImageProgram::run()
{
    Image image;

    for (auto& path: inputList)
    {
        //TODO: Check if using the ImageIO makes any difference
        image.read(path);
        pipeProc.process(image);
        image.write(outputFn);
    }

    return 0;
} // function EmImageProgram.run

void EmImageProgram::readArgs()
{
    if (hasArg("<input>"))
    {
        inputFn = getValue("<input>");
        std::cout << std::setw(10) << std::right << "Input: "
                  << inputFn << std::endl;

        // TODO: Allow to input a pattern and glob all files that match it

        inputList.push_back(inputFn);

        for (auto& path: inputList)
            ASSERT_ERROR(!Path::exists(path),
                         std::string("Input path '") + path +
                         "' does not exists!!!");
    }

    if (hasArg("<output>"))
    {
        outputFn = getValue("<output>");
        std::cout << std::setw(10) << std::right << "Output: "
                  << outputFn << std::endl;
    }

    auto& args = getArgList();

    std::cout << std::setw(10) << std::right << "Commands: "
              << args.size() << std::endl;

    std::string cmdName;

    for (auto& a: args)
    {
        auto pproc = getProcessorFromArg(a);
        if (pproc != nullptr)
            pipeProc.addProcessor(pproc);
        //else TODO: handle when the processor can not be constructed.
    }

} // function EmImageProgram.readArgs

ImageProcessor* EmImageProgram::getProcessorFromArg(const Program::Argument& arg)
{
    std::string cmdName = arg.toString();
    Type::Operation op = Type::NO_OP;

    // Check first if the argument is for a basic arithmetic operation
    if (cmdName == "add")
        op = Type::ADD;
    else if (cmdName == "sub")
        op = Type::SUB;
    else if (cmdName == "mul")
        op = Type::MUL;
    else if (cmdName == "div")
        op = Type::DIV;

    if (op != Type::NO_OP)
    {
        auto pproc = new ImageMathProc();
        (*pproc)[ImageMathProc::OPERATION] = op;
        (*pproc)[ImageMathProc::OPERAND] = String::toFloat(arg.get(1));

        std::cout << "Math cmd: " << cmdName << ", op: " << (char)op << ", operand: "
                  << (*pproc)[ImageMathProc::OPERAND] << std::endl;

        return pproc;
    }

    return nullptr;
}


int main (int argc, const char **argv)
{
    return EmImageProgram().main(argc, argv);
}
