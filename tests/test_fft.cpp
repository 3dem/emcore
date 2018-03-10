//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/os/filesystem.h"
#include "em/proc/fft.h"
#include "em/base/legacy.h"


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

//            size_t counter = 0;
//
//            FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(fImg, fImg.getDim())
//            {
//                int r2 = jp*jp + ip*ip + kp*kp;
//                if (ip != i || kp != k)
//                {
//                std::cout << "Pyshical coords: (" << i << ", " << j << ", " << k << ") " << std::endl; //<<DIRECT_A3D_ELEM(m, k, i, j) << std::endl;
//                std::cout << "        logical: (" << ip << ", " << jp << ", " << kp << ") " << std::endl;
//                std::cout << "       distance: " << sqrt(r2) << std::endl << std::endl;
//                }
//                ++counter;
//            }
//
//            std::cout << "FT: " << fImg << std::endl;

            ft.backward(fImg, rImg);
            rImg.write(ImageLocation("stack2D_img1_128.mrc", 1));

            Image rImg2;
            ft.scale(rImg, rImg2, 64);
            rImg2.write(ImageLocation("stack2D_img1_64px.mrc", 1));

            ft.scale(rImg, rImg2, 256);
            rImg2.write(ImageLocation("stack2D_img1_256.mrc", 1));



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