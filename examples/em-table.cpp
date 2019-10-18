// A simple program

#include <iostream>
#include <iomanip>
#include <math.h>

#include "emc/base/error.h"
#include "emc/os/filesystem.h"
#include "emc/base/image.h"
#include "emc/base/table.h"
#include "emc/proc/program.h"
#include "emc/proc/processor.h"


using namespace emcore;

static const char USAGE[] =
        R"(em-table.

    Usage:
      em-table <input> [<output>]

    Options:
      <input>       An input file of a metadata file.
      <output>      The output file
      -h --help     Show this screen.
      --version     Show version.
)";


class EmTableProgram: public Program
{
public:
    virtual std::string getName() const override
    {
        return "em-table";
    } // function getName

    virtual std::string getUsage() const override
    {
        return USAGE;
    }

    virtual StringVector getCommands() const override
    {
        return {};
    }

protected:
    virtual int run() override ;
    virtual void readArgs() override ;

private:
    std::string inputFn = "";
    std::string outputFn = "";
    ImagePipeProc pipeProc;
    StringVector inputList;

}; // class EmTableProgram


// ---------------------- Implementation -------------------------------

void EmTableProgram::readArgs()
{
    inputFn = "";
    std::string inputTable = "";
    outputFn = "";

    inputFn = getValue("<input>");
    std::cout << std::setw(10) << std::right << "Input: "
              << inputFn << std::endl;

    if (inputFn.find('@') != std::string::npos)
    {
        auto parts = String::split(inputFn.c_str(), '@');
        inputTable = parts[0];
        inputFn = parts[1];
    }
    ASSERT_ERROR(!Path::exists(inputFn),
                 String::join({"Input path '", inputFn, "' does not exists!!!"}));

    TableFile tio;
    tio.open(inputFn);
    std::cout << std::setw(10) << std::right << "Tables: " << std::endl;

    auto tables = tio.getTableNames();
    for (auto &name: tables)
        std::cout << "       " << name << std::endl;

    Table t;
    if (inputTable.empty())
        inputTable = tables[0];

    std::cout << " Reading table: " << inputTable << std::endl;
    tio.read(inputTable, t);

    if (hasArg("<output>"))
    {
        outputFn = getValue("<output>");
        std::cout << " Writing table: " << inputTable << " to: "
                  << outputFn << std::endl;
        auto tio2 = TableFile();
        tio2.open(outputFn, File::TRUNCATE);
        tio2.write(inputTable, t);
    }
    else
    {
        std::cout << "  count: " << t.getSize() << std::endl;
        std::cout << "DEBUG: " << std::endl << t << std::endl;
    }

} // function EmTableProgram.readArgs

int EmTableProgram::run()
{
    return 0;
} // function EmTableProgram.run




int main (int argc, const char **argv)
{
    return EmTableProgram().main(argc, argv);
}
