// A simple program

#include <iostream>
#include <iomanip>
#include <math.h>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/image/image.h"
#include "em/proc/program.h"
#include "em/proc/processor.h"


using namespace em;

static const char USAGE[] =
        R"(em-image.

    Usage:
      em-image (<input> | create <xdim> [<ydim> [<zdim>]])
                       [(add|sub|mul|div) <file_or_value>         |
                         flip (x|y|z)                             |
                         crop <left> [<top> [<right> [<bottom>]]] |
                         window <point1> <point2>                 |
                         shift (x|y|z) <value>                    |
                         rotate <value>                           |
                         rotate (x|y|z) <value>                   |
                         scale  <scale_factor>                    |
                         scale angpix <old> <new>                 |
                         scale (x|y|z) <new_dim>                  |
                        (lowpass|highpass) <freq>                 |
                        (bandpass <low_freq> <high_freq>)
                       ]... [<output>]

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
    if (outputFn.empty())
        return 0;

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
    inputFn = "";
    outputFn = "";

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

    if (hasArg("<output>"))
    {
        outputFn = getValue("<output>");
        std::cout << std::setw(10) << std::right << "Output: "
                  << outputFn << std::endl;
    }
    else
    {
        std::cout << "pipeProc.getSize: " << pipeProc.getSize() << std::endl;

        // Handle the case when the output is not defined
        ASSERT_ERROR(pipeProc.getSize() > 0,
                     "Output should be specified if performing any operation.")
        Image image;

        for (auto& path: inputList)
        {
            //FIXME: Retrieve image header info from ImageIO to avoid reading
            // the image if not necessary
            image.read(path);
            std::cout << image << std::endl;
        }
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

        std::cout << ">>> Cmd: " << cmdName << ", op: " << (char)op << std::endl;

    if (op != Type::NO_OP)
        return new ImageMathProc({{ImageMathProc::OPERATION, op},
                                  {ImageMathProc::OPERAND,
                                   String::toFloat(arg.get(1))}
                                 });

    return nullptr;
}


int main (int argc, const char **argv)
{
    return EmImageProgram().main(argc, argv);
}
