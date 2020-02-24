// A simple program

#include <iostream>
#include <iomanip>
#include <math.h>

#include "emc/base/error.h"
#include "emc/os/filesystem.h"
#include "emc/base/image.h"
#include "emc/base/table.h"
#include "emc/base/timer.h"
#include "emc/proc/program.h"
#include "emc/proc/processor.h"


using namespace emcore;

static const char USAGE[] =
        R"(em-table.

    Usage:
      em-table <input> [<output>] [--info]

    Options:
      <input>       An input file of a metadata file.
      <output>      The output file
      -h --help     Show this screen.
      --version     Show version.
      --info        Print information about the metadata files.
                    For each table it will print table name, columns
                    and number of elements in the table.
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

/**
 * Print info about the input metadata file:
 *  Tables, labels, and number of elements.
 * @param inputFn
 */
void metadataInfo(const std::string& inputFn)
{
    TableFile tio;
    tio.open(inputFn);
    std::cout << std::setw(10) << std::right << "Tables: " << std::endl;

    auto tables = tio.getTableNames();
    for (auto &name: tables)
    {
        Timer timer;
        timer.tic();
        Table t;
        tio.read(name, t);
        auto e = timer.elapsed();

        std::cout << "    " << name << std::endl;
        std::cout << "        columns: ";
        for (auto &col: t.getColumns())
            std::cout << " " << col.getName();
        std::cout << std::endl
                  << "        rows: " << t.getSize() << std::endl;
        std::cout << "        elapsed: " << e.count() << " secs. " << std::endl;
    }
}


void EmTableProgram::readArgs()
{
    inputFn = "";
    std::string inputTable = "";
    outputFn = "";

    inputFn = getArg("<input>");
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

    if (hasArg("--info"))
        metadataInfo(inputFn);
    else
    {
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
            outputFn = getArg("<output>");
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
