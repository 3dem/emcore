//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/image/image.h"


using namespace em;

TEST(Image, Constructor) {

    //Image
    Image img(ArrayDim(10, 10), em::TypeDouble);
    ObjectDict &header = img.getHeader();
    header["x"] = 10;
    header["y"] = 20.5;
    // The following one is make it crash
    header["filename"] = std::string("/path/to/image/");
    std::cout << img << std::endl;

    ASSERT_TRUE(ImageIO::has("spi"));
    ASSERT_TRUE(ImageIO::has("spider"));
    ImageIO * spiderIO = ImageIO::get("spi");
    ASSERT_EQ(spiderIO->getName(), "spider");

} // TEST(ArrayTest, Constructor)

TEST(ImageMrcIO, Read) {

    ASSERT_TRUE(ImageIO::has("mrc"));
    ASSERT_TRUE(ImageIO::has("mrcs"));
    ImageIO * mrcIO = ImageIO::get("mrc");
    ASSERT_EQ(mrcIO->getName(), "mrc");

    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try {
            std::string root(testDataPath);

            fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
            fileDims["emx/alignment/Test2/stack2D.mrc"] = ArrayDim(128, 128, 1, 100);

            for (auto &pair: fileDims) {
                Image img;
                loc.index = 1;
                loc.path = root + pair.first;
                std::cout << "Before reading. " << std::endl;

                mrcIO->read(loc, img);
                std::cout << img << std::endl;
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
                ASSERT_TRUE(img.getDimensions() == imgDim);
                ASSERT_TRUE(mrcIO->getDimensions() == pair.second);
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

} // TEST(ArrayTest, Constructor)


TEST(ImageSpiderIO, Read) {

    ASSERT_TRUE(ImageIO::has("spider"));
    ImageIO * spiIO = ImageIO::get("spi");
    ASSERT_EQ(spiIO->getName(), "spider");


    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try
        {
            std::string root(testDataPath);

            fileDims["emx/alignment/testAngles/sphere_128.vol"] = ArrayDim(128, 128, 128, 1);
            fileDims["emx/alignment/testAngles/proj.spi"] = ArrayDim(128, 128, 1, 1);
            fileDims["emx/alignment/testAngles/projections.stk"] = ArrayDim(128, 128, 1, 5);


            for (auto &pair: fileDims)
            {
                Image img;
                loc.index = 1;
                loc.path = root + pair.first;
                spiIO->read(loc, img);
                std::cout << "Back in test" << std::endl;
                std::cout << img << std::endl;
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
                ASSERT_TRUE(img.getDimensions() == imgDim);
                ASSERT_TRUE(spiIO->getDimensions() == pair.second);
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

} // TEST(ArrayTest, Constructor)

