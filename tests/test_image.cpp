//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/image/image.h"


using namespace em;

TEST(ImageLocation, Basic)
{
    // Image Location can be constructed from string and index is ALL
    std::string path("/path/to/image");
    ImageLocation loc2("/path/to/image");
    ASSERT_EQ(loc2.index, ImageLocation::ALL);
    ASSERT_EQ(loc2.path, path);

    ImageLocation loc3(path, 2);
    ASSERT_EQ(loc3.path, path);
    ASSERT_NE(loc2, loc3);
    loc3.index = 0;
    ASSERT_EQ(loc2, loc3);

    ASSERT_EQ(loc2.toString(), std::string("(/path/to/image, ALL)"));
} // TEST(ImageLocation, Basic)


TEST(ImageIO, Impl)
{
    ASSERT_TRUE(ImageIO::hasImpl("spi"));
    ASSERT_TRUE(ImageIO::hasImpl("spider"));
    ImageIO spiderIO = ImageIO("spi");

    ASSERT_TRUE(ImageIO::hasImpl("mrc"));
    ASSERT_TRUE(ImageIO::hasImpl("mrcs"));
    ImageIO mrcIO = ImageIO("mrc");
} // TEST(ImageIO, Impl)


TEST(Image, Constructor)
{
    //Image
    Image img(ArrayDim(10, 10), em::typeDouble);
    ObjectDict &header = img.getHeader();
    header["x"] = 10;
    header["y"] = 20.5;
    header["filename"] = std::string("/path/to/image/");
    std::cout << img << std::endl;
} // TEST(Image, Constructor)


TEST(ImageMrcIO, Read)
{
    ImageIO mrcIO = ImageIO("mrc");
    // ASSERT_EQ(mrcIO.getName(), "mrc");

    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try {
            std::string root(testDataPath);

            fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
            std::string stackFn = "emx/alignment/Test2/stack2D.mrc";
            ArrayDim stackDim(128, 128, 1, 100);
            // fileDims[stackFn] = stackDim;

            for (auto &pair: fileDims)
            {
                Image img;
                loc.index = 1;
                loc.path = root + pair.first;
                std::cout << ">>> Reading image: " << loc << std::endl;

                img.read(loc);
                std::cout << img << std::endl;
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
                ASSERT_EQ(img.getDim(), imgDim);
            }

            // Use mrcIO2 for writing individual images
            ImageIO mrcIO2 = ImageIO("mrc");
            mrcIO.open(root + stackFn);
            Image img;
            char suffix[4];
            std::string imgFn;
            ArrayDim imgDim(stackDim);
            imgDim.n = 1;

            for (size_t i = 1; i < 11; ++i)
            {
                mrcIO.read(i, img);
                snprintf (suffix, 4, "%03d", (int)i);
                imgFn = std::string("image") + suffix + ".mrc";
                std::cout << ">>> Writing image: " << imgFn << std::endl;
                mrcIO2.open(imgFn, ImageIO::TRUNCATE);
                mrcIO2.createFile(imgDim, img.getType());
                mrcIO2.write(1, img);
                mrcIO2.close();

            }
            mrcIO.close();
        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping image format tests, EM_TEST_DATA not defined in "
                     "environment. " << std::endl;
    }

} // TEST(ImageMrcIO, Read)


TEST(ImageSpiderIO, Read)
{

    ASSERT_TRUE(ImageIO::hasImpl("spider"));
    ImageIO spiIO = ImageIO("spi");
    //ASSERT_EQ(spiIO->getName(), "spider");


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
                std::cerr << ">>>>>>>>>>>>>>>>> Reading " << pair.first << std::endl;
                loc.path = root + pair.first;
                img.read(loc);
                std::cout << "Back in test" << std::endl;
                std::cout << img << std::endl;
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
                ASSERT_EQ(img.getDim(), imgDim);
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

} // TEST(ImageSpiderIO, Read)


TEST(ImageIO, Create)
{

    StringVector exts = {"mrc", "spi"};
    const size_t DIM = 16; // 128

    for (auto ext: exts)
    {
        ImageIO imgio = ImageIO(ext);
        std::cout << "Using IO: " << ext << std::endl;

        std::string fn;
        // Write a single image
        fn = "image-single." + ext;
        imgio.open(fn.c_str(), ImageIO::TRUNCATE);
        imgio.createFile(ArrayDim(DIM, DIM, 1, 1), em::typeFloat);
        imgio.close();

        // Write a stack of images
        fn = "image-stack." + ext;
        imgio.open(fn.c_str(), ImageIO::TRUNCATE);
        imgio.createFile(ArrayDim(DIM, DIM, 1, 100), em::typeFloat);

        Image img(ArrayDim(DIM, DIM, 1, 1), em::typeFloat);
        auto av = img.getView<float>();
        av.assign(200);
        imgio.write(1, img);
        imgio.close();

    }
} // TEST(ImageIO, Create)
