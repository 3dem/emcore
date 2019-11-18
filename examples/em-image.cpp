// A simple program

#include <iostream>
#include <iomanip>
#include <set>
#include <utility>
#include <math.h>

#include "emc/base/error.h"
#include "emc/os/filesystem.h"
#include "emc/base/image.h"
#include "emc/proc/program.h"
#include "emc/proc/processor.h"
#include "emc/proc/stats.h"

using namespace emcore;

static const char USAGE[] =
        R"(em-image.

    Usage:
      em-image --formats
      em-image create <create_dims> <output>
      em-image <input> [--stats]
      em-image <input> <output>
      em-image <input> ((add|sub|mul|div) <file_or_value>   |
                         flip <flip_axis>                   |
                         crop <crop_values>                 |
                         window <window_p1> <window_p2>     |
                         shift <shift_arg>                  |
                         rotate <rotate_arg>                |
                         scale  <scale_arg>                 |
                       )... <output>

    Options:
      <input>               An input file or a pattern matching many files.
      -h --help             Show this screen.
      --version             Show version.
      --formats             Print the list of available image formats
      flip <flip_axis>      Flip the images in this axis.
                            <flip_axis> should be: x, y, or z
)";



/*
 * |
                         [(add|sub|mul|div) <file_or_value>]
                         flip <flip_axis>                   |
                         crop <crop_values>                 |
                         window <window_p1> <window_p2>     |
                         shift <shift_arg>                  |
                         rotate <rotate_arg>                |
                         scale  <scale_arg>                 |
                         filter <filter_arg>
 */
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

private:
    std::string inputFn = "";
    std::string outputFn = "";
    std::string outputFormat;
    Type outputType;
    ImagePipeProc pipeProc;
    StringVector inputList;

    void processCommand(const StringVector& args,
                        size_t startIndex, size_t endIndex);

    ImageProcessor* createProcessorFromCommand(const Command &command);

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
        std::cerr << "DEBUG:   after hasArg(<input>) ..." << std::endl;
        inputFn = getArg("<input>");
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

    auto const &commandList = getCommandList();

    for (auto &cmd : getCommandList())
    {
        auto proc = createProcessorFromCommand(cmd);
        if (proc != nullptr)
            pipeProc.addProcessor(proc);
        //else TODO: handle when the processor can not be constructed.
    }

} // function EmImageProgram.readArgs

ImageProcessor* EmImageProgram::createProcessorFromCommand(const Command &cmd)
{
    std::string cmdName = cmd.getName();

    Type::Operation op = Type::NO_OP;
    ImageProcessor *imgProc = nullptr;

    // Check first if the argument is for a basic arithmetic operation
    if (cmdName == "add")
        op = Type::ADD;
    else if (cmdName == "sub")
        op = Type::SUB;
    else if (cmdName == "mul")
        op = Type::MUL;
    else if (cmdName == "div")
        op = Type::DIV;

    if (op != Type::NO_OP)  // Case of an arithmetic operation
    {
        std::cout << "DEBUG: >>> Cmd: " << cmdName << ", op: " << (char)op << std::endl;
        std::cout << "DEBUG:     Value: " << cmd.getArg("<file_or_value>") << std::endl;
        imgProc = new ImageMathProc(
                {{ImageMathProc::OPERATION, op},
                {ImageMathProc::OPERAND, cmd.getArgAsFloat("<file_or_value>")}});
    }
    else
    {
        ObjectDict params;

        if (cmdName == "scale")
        {
//            auto arg1 = arg.getString(1);

            //TODO: Correctly parse now scale parameters
//            if (arg1 == "angpix")
//                params = {{"angpix_old", arg.getFloat(2)},
//                          {"angpix_new", arg.getFloat(3)}};
//            else if (arg1 == "x")
//                params = {{"newdim_x", arg.getFloat(2)}};
//            else if (arg1 == "y")
//                params = {{"newdim_y", arg.getFloat(2)}};
//            else
//                params = {{"factor", arg.getFloat(1)}};

            imgProc = new ImageScaleProc(params);
        }
        else if (cmdName == "crop")
        {
            params[ImageProcessor::OPERATION] = ImageWindowProc::OP_CROP;

            //TODO: Correctly parse crop parameters
//            params["left"] = arg.getInt(1);
//
//            auto n = arg.getSize();
//            if (n > 1)
//                params["top"] = arg.getInt(2);
//            if (n > 2)
//                params["right"] = arg.getInt(3);
//            if (n > 3)
//                params["bottom"] = arg.getInt(4);

            imgProc = new ImageWindowProc(params);
        }
        else if (cmdName == "window")
        {
            imgProc = new ImageWindowProc(params);
        }
    }
    return imgProc;
}

/** Helper function to throw errors related to formats */
void throwFormatError(StringVector msgParts)
{
    msgParts.push_back("\nRun 'em-image --formats' to check valid formats.");
    THROW_ERROR(String::join(msgParts));
}

void EmImageProgram::parseOutputString()
{
    auto formatTypes = ImageFile::getFormatTypes();
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

    auto output = getArg("<output>");

    auto parts = String::split(output, ':');
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
        ImageFile imgio;
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
    auto formatTypes = ImageFile::getFormatTypes();

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
    ImageFile inputIO;

    auto doProcess = pipeProc.getSize() > 0;
    auto hasOutput = hasArg("<output>");

    ASSERT_ERROR(doProcess && !hasOutput,
                 "Please provide output value.");

    if (hasOutput)  // Convert input
    {

        parseOutputString();
        std::cout << "Output " << std::endl
                  << "     file: " << outputFn << std::endl
                  << "   format: " << outputFormat << std::endl
                  << "     type: " << outputType.getName() << std::endl;

        ImageFile outputIO;

        for (auto& path: inputList)
        {
            inputIO.open(path);
            outputIO.open(outputFn, File::TRUNCATE);

            auto adim = inputIO.getDim();
            outputIO.createEmpty(adim, outputType);

            for (int i = 1; i <= adim.n; ++i)
            {
                std::cout << "Reading " << i << " from " << path << std::endl;
                inputIO.read(i, inputImage);

                std::cout << "Writing " << i << " to " << outputFn << std::endl;
                if (doProcess)
                {
                    pipeProc.process(inputImage, outputImage);
                    outputIO.write(i, outputImage);
                }
                else
                {
                    // Just convert if necessary
                    outputIO.write(i, inputImage);
                }
                //outputImage.copy(inputImage, outputType);
            }
            inputIO.close();
            outputIO.close();
        } // Loop over all input files
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
