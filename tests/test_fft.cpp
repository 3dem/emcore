//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/os/filesystem.h"
#include "em/proc/fft.h"


using namespace em;


TEST(FourierTransformer, Basic)
{
    FourierTransformer ft;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try
        {
            std::string root(testDataPath);

            //fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
            std::string stackFn = root + "emx/alignment/Test2/stack2D.mrc";
            //std::string stackFn = Path::join({root, "emx", "alignment",
            //                                  "Test2", "stack2D.mrc"});
            ArrayDim stackDim(128, 128, 1, 100);
            ImageLocation loc(stackFn, 1);
            // fileDims[stackFn] = stackDim;

            Image rImg, fImg;
            rImg.read(loc);
            std::cout << "img type: " << rImg.getType().getName() << std::endl;
            ft.forward(rImg, fImg);

            std::cout << "FT: " << fImg << std::endl;

        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping FFT tests, EM_TEST_DATA not defined in "
                     "environment. " << std::endl;
    }
} // TEST FourierTransformer.Basic