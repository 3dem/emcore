// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "docopt.h"

#include "em/base/error.h"
#include "em/image/image.h"

using namespace em;

static const char USAGE[] =
        R"(Naval Fate.

    Usage:
      naval_fate ship new <name>...
      naval_fate ship <name> move <x> <y> [--speed=<kn>]
      naval_fate ship shoot <x> <y>
      naval_fate mine (set|remove) <x> <y> [--moored | --drifting]
      naval_fate (-h | --help)
      naval_fate --version

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --speed=<kn>  Speed in knots [default: 10].
      --moored      Moored (anchored) mine.
      --drifting    Drifting mine.
)";


int main (int argc, char *argv[])
{

    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                               { argv + 1, argv + argc },
                                                               true,               // show help if requested
                                                               "Naval Fate 2.0");  // version string

    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    return 0;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try {
            std::string root(testDataPath);
            ImageIO mrcIO = ImageIO("mrc");

            ImageLocation loc;
            std::map<std::string, ArrayDim> fileDims;

            fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
            std::string stackFn = "emx/alignment/Test2/stack2D.mrc";
            ArrayDim stackDim(128, 128, 1, 100);
            fileDims[stackFn] = stackDim;

            // Use mrcIO2 for writing individual images
            StringVector exts = {"mrc", "spi"};
            std::vector<ConstTypePtr> types = {em::TypeFloat, em::TypeInt16};

            for (auto ext: exts)
            {
                ImageLocation imgLoc;
                imgLoc.index = 1;

                ImageIO imgio2 = ImageIO(ext);
                //mrcIO.open(root + stackFn);
                Image img;
                char suffix[5];
                std::string imgFn;
                ArrayDim imgDim(stackDim);
                imgDim.n = 1;
                StringVector filenames;

                mrcIO.open(root + stackFn);
                // Write images in single files (only 10)
                for (size_t i = 1; i < 11; ++i)
                {
                    mrcIO.read(i, img);
                    snprintf(suffix, 5, "%03d.", (int) i);
                    imgLoc.path = std::string("image") + suffix + ext;
                    filenames.push_back(imgLoc.path);

                    std::cout << ">>> Writing image: " << imgLoc.path << std::endl;
//                    imgio2.open(imgLoc.path, ImageIO::TRUNCATE);
//                    imgio2.createFile(imgDim, em::TypeFloat);
//                    imgio2.write(1, img);
//                    imgio2.close();
                    img.write(imgLoc);
                }

                size_t count = 0;
                imgio2.open(std::string("images-stack.") + ext, ImageIO::TRUNCATE);
                imgDim.n = filenames.size();
                imgio2.createFile(imgDim, types[0]);

                // Read the images previously written and make a single stack
                for (auto fn: filenames)
                {
                    imgLoc.path = fn;
                    img.read(imgLoc);
                    ++count;
                    imgio2.write(count, img);
                }
                imgio2.close();
            }
        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping image format tests, EM_TEST_DATA not defined "
                     "in environment. " << std::endl;
    }

    return 0;
}
