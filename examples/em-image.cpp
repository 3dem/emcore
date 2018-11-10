// A simple program

#include <iostream>
#include <iomanip>
#include <set>
#include <utility>
#include <math.h>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/base/image.h"
#include "em/proc/program.h"
#include "em/proc/processor.h"
#include "em/proc/stats.h"

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
                       [--stats]

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
    std::string outputFormat;
    Type outputType;
    ImagePipeProc pipeProc;
    StringVector inputList;

    void processCommand(const StringVector& args,
                        size_t startIndex, size_t endIndex);

    ImageProcessor* getProcessorFromArg(const Program::Argument& arg);

    /**
     * Parse the output string (path:format:type) where the values can be separated by :
     * First value should be the path, second and third can be the format and type
     * Both format and type are optional and the order can be interchanged.
     * If not provided, the empty string will be returned
     * @return A vector with the different parts. If the format is provided, it
     * will be in the second position. If the type is provided, it will be in the
     * third position.
     */
    void parseOutputString();
}; // class EmImageProgram


// ---------------------- Implementation -------------------------------


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
            for (size_t i = 0; i < glob.getSize(); ++i)
                inputList.push_back(glob.getResult(i));

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

//    std::cout << std::setw(10) << std::right << "Commands: "
//              << args.size() << std::endl;

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

/** Helper function to throw errors related to formats */
void throwFormatError(StringVector msgParts)
{
    msgParts.push_back("\nRun 'em-image --formats' to check valid formats.");
    THROW_ERROR(String::join(msgParts));
}

void EmImageProgram::parseOutputString()
{
    auto formatTypes = ImageIO::getFormatTypes();
    StringTypeMap typeNames = {{"int8", typeInt8},
                               {"uint8", typeUInt8},
                               {"int16", typeInt16},
                               {"uint16", typeUInt16},
                               {"int32", typeInt32},
                               {"uint32", typeUInt32},
                               {"int64", typeInt64},
                               {"uint64", typeUInt64},
                               {"float", typeFloat},
                               {"double", typeDouble}};

    auto parts = String::split(getValue("<output>"), ':');
    auto n = parts.size();

    if (n == 3)
    {
        // Nothing else to do here, format is already in second position
        if (formatTypes.find(parts[1]) != formatTypes.end()) {}
            // Now we need to switch the positions to put format in second position
        else if (formatTypes.find(parts[2]) != formatTypes.end())
            std::swap(parts[1], parts[2]);
        else
            throwFormatError({"Format can not be found in position 2 (",
                              parts[1], ") or 3 (", parts[2], ")"});

        if (typeNames.find(parts[2]) == typeNames.end())
            throwFormatError({"Invalid type name (", parts[2], ")"});
    }
    else if (n == 2)
    {
        if (formatTypes.find(parts[1]) != formatTypes.end())
            parts.push_back(Path::getExtension(parts[0]));  // empty type
        else if (typeNames.find(parts[1]) != typeNames.end())
            parts.insert(parts.begin() + 1, "");  // empty format in second position
        else
            throwFormatError({"Format or type can not be found in "
                              "position 2 (", parts[1], ")."});
    }
    else if (n == 1)
    {
        parts.push_back("");  // empty format
        parts.push_back("");  // empty type
    }
    else
        THROW_ERROR("The output should not have more than two : characters");

    outputFn = parts[0];
    outputFormat = parts[1];

    if (outputFormat.empty())
    {
        // Infer the format from the output or input files
        auto ext = Path::getExtension(outputFn);
        outputFormat = ext.empty() ? Path::getExtension(inputList[0]) : ext;
    }

    if (parts[2].empty())
    {
        ImageIO imgio;
        imgio.open(inputList[0]);
        outputType = imgio.getType();
        imgio.close();
    }
    else
    {
        auto it = typeNames.find(parts[2]);
        if (it == typeNames.end())
            throwFormatError({"Invalid output type (", parts[2], ")"});
        outputType = it->second;

    }
} // function EmImageProgram.parseOutputString

int EmImageProgram::run()
{
    auto formatTypes = ImageIO::getFormatTypes();

    if (hasArg("--formats"))
    {
        std::cout << "Supported formats: " << std::endl;

        for (const auto& kv: formatTypes)
        {
            std::cout << kv.first << ": ";
            for (const auto& type: kv.second)
                std::cout << type.getName() << " ";
            std::cout << std::endl;
        }
        return 0;
    }

    Image inputImage, outputImage;
    ImageIO inputIO;

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

        parseOutputString();
        std::cout << "Output " << std::endl
                  << "     file: " << outputFn << std::endl
                  << "   format: " << outputFormat << std::endl
                  << "     type: " << outputType.getName() << std::endl;

        ImageIO outputIO(outputFormat);

        for (auto& path: inputList)
        {
            inputIO.open(path);
            inputIO.read(1, inputImage);

            outputIO.open(outputFn, File::TRUNCATE);
            if
            //outputImage.copy(inputImage, typeFloat);
            outputIO.write(1, inputImage);
        }
    }
    else  // Just print information about the input images
    {
        auto hasStats = hasArg("--stats");
        Stats stats;

        for (const auto& path: inputList)
        {
            inputIO.open(path);
            std::cout << std::endl;
            inputIO.toStream(std::cout, 2);

            if (hasStats)
            {
                inputIO.read(1, inputImage);
                stats = Stats::compute(inputImage);
                std::cout << "Stats: " << stats << std::endl;
            }
            inputIO.close();
        }
    }

    return 0;
} // function EmImageProgram.run


int main (int argc, const char **argv)
{
    return EmImageProgram().main(argc, argv);
}
