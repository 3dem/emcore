// A simple program

#include <iostream>
#include <iomanip>
#include <math.h>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/base/image.h"
#include "em/proc/program.h"
#include "em/proc/processor.h"


using namespace em;

static const char USAGE[] =
        R"(em-image.

    Usage:
      em-image (<input> | create <xdim> [<ydim> [<zdim>]] | [--formats])
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
                       [--oformat <oformat>]

    Options:
      <input>     An input file or a pattern matching many files.
      <output>    An output file or a suffix when many files are produced.
      -h --help   Show this screen.
      --version   Show version.
      --formats   Print the list of available image formats
      --oformat <oformat>  Specify the output format (e.g 'mrc' or 'mrc:int8')
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
    if (hasArg("--formats"))
    {
        std::cout << "Supported formats: " << std::endl;

        for (const auto& kv: ImageIO::getFormatTypes())
            std::cout << kv.first << std::endl;
    }


    Image inputImage, outputImage;
    ImageIO inputIO, outputIO;

    auto doProcess = pipeProc.getSize() > 0;
    auto hasOutput = hasArg("<output>");

    ASSERT_ERROR(doProcess && !hasOutput,
                 "Please provide output option.")

    if (doProcess)  // Do some processing on the images
    {
        THROW_ERROR("Right now only using em-image for conversions. "
                            "Processing will come soon!!!");

//        for (auto& path: inputList)
//        {
//            imgIO.open(path);
//            imgIO.read(1, inputImage);
//            pipeProc.process(inputImage, outputImage);
//            //imgIO.write();
//        }
    }
    else if (hasOutput)  // Convert input
    {
        std::cout << "has output..." << std::endl;

        if (hasArg("--oformat"))
        {
            std::cout << "oformat: " << getValue("--oformat") << std::endl;
        }
        for (auto& path: inputList)
        {
            //TODO: Check if using the ImageIO makes any difference
//            image.read(path);
//            if (pipeProc.getSize() > 0)
//                pipeProc.process(image);
//            image.write(outputFn);
        }
    }
    else  // Just print information about the input images
    {

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

        if (inputFn.find('*') != std::string::npos ||
            inputFn.find('?') != std::string::npos)
        {
            Glob glob(inputFn);

            ASSERT_ERROR(glob.getSize() == 0,
                         "There are not files matching input pattern.");
        }
        else
        {
            inputList.push_back(inputFn);

            ASSERT_ERROR(!Path::exists(inputFn), "Input path does not exists!!!");
        }
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

//    if ()
//    {
//        outputFn = getValue("<output>");
//        std::cout << std::setw(10) << std::right << "Output: "
//                  << outputFn << std::endl;
//    }
//    else
//    {
//        // Handle the case when the output is not defined
//        ASSERT_ERROR(pipeProc.getSize() > 0,
//                     "Output should be specified if performing any operation.")
//        ImageIO imgIO;
//
//        for (auto& path: inputList)
//        {
//            imgIO.open(path);
//            imgIO.toStream(std::cout, 2);
//        }
//    }

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
