//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/os/filesystem.h"
#include "em/proc/fft.h"
#include "em/base/legacy.h"
#include "em/math/functions.h"


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

Image computeGaussianImage(int size, int gaussSize, float gauss_max_value=0.1)
{
    size += size % 2; // make even
    Image img(ArrayDim(size, size), typeFloat);
    // Make a Gaussian reference. sigma is 1/6th of the particle size,
    // such that 3 sigma is at the image edge
    GaussianFunc<float> gauss((float)gaussSize/6);
    float normgauss = gauss(0.);

    LegacyArray<float> array(img);
    array.setXmippOrigin();

    FOR_ALL_ELEMENTS_IN_ARRAY2D(array)
        {
            double r = sqrt((float)(i*i + j*j));
            A2D_ELEM(array, i, j) = gauss_max_value * gauss(r) / normgauss;
        }

    return img;
}

TEST(FourierTransformer, Crosscorrelation)
{
    FourierTransformer ft;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try
        {
        std::string root(testDataPath);

        //fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
        //std::string micFn = root + "xmipp_tutorial/micrographs/BPV_1386.mrc";
        std::string micFn = root + "relion_tutorial/micrographs/006.mrc";
        //std::string stackFn = Path::join({root, "emx", "alignment",
        //                                  "Test2", "stack2D.mrc"});
        // ArrayDim micDim(9216, 9441, 1, 1);
        ArrayDim micDim(1024, 1024, 1, 1);
        ImageLocation loc(micFn, 1);
        // fileDims[stackFn] = stackDim;

        size_t ccSize = 512;
        Image rMic(micDim, typeFloat), fMic;
        Image rRef = computeGaussianImage(ccSize, 40, 10000), fRef;
        Image ccImg;

        std::cout << ">>> Reading image: " << micFn << std::endl;
        rMic.read(loc);
        std::cout << "   Type: " << rMic.getType().getName() << std::endl;
        std::cout << "    Dim: " << rMic.getDim() << std::endl;

        LegacyArray<float> arMic(rMic);
        FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(arMic)
        {
            DIRECT_MULTIDIM_ELEM(arMic, n) *= -1;
        }

        rMic.write(ImageLocation("006_inverted.mrc", 1));

        ft.forward(rMic, fMic);
        ft.windowFT(fMic, ccSize);


        ft.forward(rRef, fRef);
        // ft.windowFT(fRef, ccSize);


        rRef.write(ImageLocation("reference1.mrc", 1));
        //rRef.resize(ArrayDim(ccSize, ccSize), typeFloat);
        ft.backward(fRef, rRef);

        rRef.write(ImageLocation("reference2.mrc", 1));

        LegacyArray<std::complex<float>> afMic(fMic), afRef(fRef);

        // Now multiply template and micrograph to calculate the cross-correlation
        FOR_ALL_DIRECT_ELEMENTS_IN_MULTIDIMARRAY(afRef)
        {
            DIRECT_MULTIDIM_ELEM(afRef, n) = conj(DIRECT_MULTIDIM_ELEM(afRef, n)) * DIRECT_MULTIDIM_ELEM(afMic, n);
        }


        ft.backward(fRef, rRef);
        rRef.write(ImageLocation("cc.mrc", 1));


//        // If we're not doing shrink, then Faux is bigger than Faux2!
//        windowFourierTransform(Faux, Faux2, workSize);
//        transformer.inverseFourierTransform(Faux2, Maux);
//        CenterFFT(Maux, false);

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
} // TEST FourierTransformer.Crosscorrelation

TEST(FourierTransformer, shift)
{
    FourierTransformer ft;
    Image img1, img2, img3;

    for (auto adim: {ArrayDim(4), ArrayDim(5),
                     ArrayDim(4, 4), ArrayDim(5, 5)})
    {
        img1.resize(adim, typeFloat);
        auto array = img1.getView<float>();
        auto data = array.getData();
        std::cout << "data = " << data << std::endl;

        auto n = adim.getSize();
        std::cout << "Number of elements: " << n << std::endl;

        // Fill it with consecutive numbers
        std::cout << "Before: " << std::endl;
        for (size_t y = 0, i = 0; y < adim.y; ++y)
        {
            for (size_t x = 0; x < adim.x; ++x)
            {
                array(x, y) = ++i;
                std::cout << array(x, y) << " ";
            }
            std::cout << std::endl;
        }

        img2.resize(adim, typeFloat);
        ft.shift(img1, img2);

        std::cout << "After ft.shift: " << std::endl;
        auto array2 = img2.getView<float>();
        for (size_t y = 0; y < adim.y; ++y)
        {
            for (size_t x = 0; x < adim.x; ++x)
                std::cout << array2(x, y) << " ";
            std::cout << std::endl;
        }

        img3.resize(adim, typeFloat);
        ft.shift(img2, img3, FT::BACKWARD);
        auto array3 = img3.getView<float>();
        for (size_t y = 0; y < adim.y; ++y)
        {
            for (size_t x = 0; x < adim.x; ++x)
            std::cout << array3(x, y) << " ";
            std::cout << std::endl;
        }

        std::cout << std::endl << std::endl;
    }

}