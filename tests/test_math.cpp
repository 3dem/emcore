//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/os/filesystem.h"
#include "em/math/functions.h"
#include "em/image/image.h"
#include "em/base/legacy.h"


using namespace em;


TEST(GaussianFun, Basic)
{
    int n = 15;
    float mean = (float)n/2;
    float stddev = (float)n/3;

    GaussianFunc<float> gauss(stddev, mean);

    float goldArray[] = {
        4.24689e-06, 2.6563e-05, 0.000148672, 0.000744605, 0.00333709,
        0.013383, 0.0480271, 0.154228, 0.443185, 1.1396, 2.62219, 5.3991,
        9.94771, 16.401, 24.1971, 31.9448, 37.7383, 39.8942, 37.7383, 31.9448,
        24.1971, 16.401, 9.94771, 5.3991, 2.62219, 1.1396, 0.443185, 0.154228,
        0.0480271, 0.013383, 0.00333709, 0.000744605, 0.000148672, 2.6563e-05,
        4.24689e-06, 2.02529e-09};

//    for (size_t i = 0; i < n; ++i)
//        ASSERT_NEAR(goldArray[i], gauss(i) * 300, 0.001);

//    GaussianFunc<float> gauss2d(stddev, stddev, 1.5708, mean, mean);
//    Image img(ArrayDim(n, n), typeFloat);
//    auto array = img.getView<float>();
//    int x, y;
//
//    for (int j = 0; j < n; ++j)
//    {
//        for (int i = 0; i < n; ++i)
//        {
//            array(i, j) = gauss2d(i, j) * 300;
//            std::cout << array(i, j) << " ";
//        }
//        std::cout << std::endl;
//    }
//
//    std::cout << std::endl<< std::endl<< std::endl;

    //        //array(i, j) = gaussian2D((float)i, (float)j, stddev, stddev, 1.5708f,  mean, mean);
    //
    //    ImageLocation loc("gauss2d.mrc", 1);
    //    img.write(loc);
    //
    //    Image img2;
    //    img2.read(loc);
    //    auto array2 = img2.getView<float>();
    //for (int j = 0; j < n; ++j)
    //{
    //for (int i = 0; i < n; ++i)
    //{
    //std::cout << array2(i, j) << " ";
    //}
    //std::cout << std::endl;
    //}

} // TEST FourierTransformer.Basic

TEST(GaussianFunc, createImage)
{
    float angpix = 1.05f;
    float gauss_max_value = 0.1;
    int particle_diameter = 50; // A

    // Set particle boxsize to be 1.5x bigger than circle with particle_diameter
    int size =  1.5 * (particle_diameter/angpix);
    size += size % 2; // make even

    Image img(ArrayDim(size, size), typeFloat);
    // Make a Gaussian reference. sigma is 1/6th of the particle size,
    // such that 3 sigma is at the image edge
    GaussianFunc<float> gauss((float)size/6);
    float normgauss = gauss(0.);

    LegacyArray<float> array(img);
    array.setXmippOrigin();

    FOR_ALL_ELEMENTS_IN_ARRAY2D(array)
    {
        double r = sqrt((float)(i*i + j*j));
        A2D_ELEM(array, i, j) = gauss_max_value * gauss(r) / normgauss;
    }

    std::string path("gauss2d.mrc");

    Path::remove(path);
    ImageLocation loc(path, 1);
    img.write(loc);

} // TEST GaussianFunc.createImage


