// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "em/base/error.h"
#include "em/image/image.h"

using namespace em;

int main (int argc, char *argv[])
{


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

            for (auto &pair: fileDims)
            {
                Image img;
                loc.index = 1;
                loc.path = root + pair.first;
                std::cout << "Before reading. " << std::endl;

                mrcIO.read(loc, img);
                std::cout << img << std::endl;
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
            }

            // Use mrcIO2 for writing individual images
            StringVector exts = {"mrc", "spi"};

            for (auto ext: exts)
            {
                ImageIO mrcIO2 = ImageIO(ext);
                mrcIO.open(root + stackFn);
                Image img;
                char suffix[5];
                std::string imgFn;
                ArrayDim imgDim(stackDim);
                imgDim.n = 1;
                StringVector filenames;

                // Write images in single files (only 10)
                for (size_t i = 1; i < 11; ++i)
                {
                    mrcIO.read(i, img);
                    snprintf(suffix, 5, "%03d.", (int) i);
                    imgFn = std::string("image") + suffix + ext;
                    filenames.push_back(imgFn);
                    std::cout << ">>> Writing image: " << imgFn << std::endl;
                    mrcIO2.open(imgFn, ImageIO::TRUNCATE);
                    mrcIO2.createFile(imgDim, img.getType());
                    mrcIO2.write(1, img);
                    mrcIO2.close();
                }

                ImageLocation imgLoc;
                imgLoc.index = 1;
                size_t count = 0;
                mrcIO2.open(std::string("images-stack.") + ext, ImageIO::TRUNCATE);
                imgDim.n = filenames.size();
                mrcIO2.createFile(imgDim, img.getType());

                // Read the images previously written and make a single stack
                for (auto fn: filenames)
                {
                    imgLoc.path = fn;
                    mrcIO.read(imgLoc, img);
                    ++count;
                    mrcIO2.write(count, img);
                }
                mrcIO2.close();
            }
        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping image format tests, EM_TEST_DATA not defined in environment. " << std::endl;
    }

    return 0;
}
