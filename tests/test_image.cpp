//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/base/image.h"
#include "em/base/timer.h"


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
    Image img(ArrayDim(10, 10), typeDouble);
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
                mrcIO2.open(imgFn,  File::Mode::TRUNCATE);
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
        imgio.open(fn.c_str(),  File::Mode::TRUNCATE);
        imgio.createFile(ArrayDim(DIM, DIM, 1, 1), typeFloat);
        imgio.close();

        // Write a stack of images
        fn = "image-stack." + ext;
        imgio.open(fn.c_str(),  File::Mode::TRUNCATE);
        imgio.createFile(ArrayDim(DIM, DIM, 1, 100), typeFloat);

        Image img(ArrayDim(DIM, DIM, 1, 1), typeFloat);
        auto av = img.getView<float>();
        av.assign(200);
        imgio.write(1, img);
        imgio.close();

    }
} // TEST(ImageIO, Create)


TEST(Image, Performance)
{
    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try
        {
            // Load a big ~ 10k x 10k to check the performance of some
            // operations
            std::string micFn(testDataPath);
            micFn += "xmipp_tutorial/micrographs/BPV_1386.mrc";

            ImageLocation loc(micFn, 1);
            Image img, img2, img3;

            img.read(loc);
            ArrayDim adim = img.getDim();
            Type type = img.getType();
            size_t n = adim.getSize();
            std::cout << std::endl << "Image memory: " << n * type.getSize()
                                   << " bytes. " << std::endl;
            uint8_t * imgData = static_cast<uint8_t*>(img.getData());
            uint8_t * iter = nullptr, *iter2 = nullptr;

            Timer t;

            // Resize before comparing times
            t.tic();
            img2.resize(img);
            t.toc("One image ALLOCATION");
            img3.resize(img);
            std::cout << std::endl;

            //------------------ Check Assignment to ARRAY --------------------
            t.tic();
            iter = imgData;
            iter2 = static_cast<uint8_t*>(img2.getData());
            for (size_t i = 0; i < n; ++i, ++iter, ++iter2)
                *iter2 = *iter;
            t.toc("LOOP assign array");

            t.tic();
            iter = imgData;
            iter2 = static_cast<uint8_t*>(img2.getData());
            memcpy(iter2, iter, n * type.getSize());
            t.toc("MEMCPY assign array");

            t.tic();
            img3 = img;
            t.toc("IMAGE assign array");
            ASSERT_EQ(img2, img3);

            std::cout << std::endl;

            //------------------ Check Assignment to VALUE --------------------
            t.tic();
            iter2 = static_cast<uint8_t*>(img2.getData());
            for (size_t i = 0; i < n; ++i, ++iter2)
                *iter2 = 100;
            t.toc("LOOP assign value");

            t.tic();
            // FIXME: Why the following line does not work
            //img3 = Object(100);
            Array * array = &img3;
            *array = 100;
            t.toc("IMAGE assign value");
            ASSERT_EQ(img2, img3);

            std::cout << std::endl;

            //------------------ Check Multiplication by ARRAY ---------------
            img3 = img2 = img;

            t.tic();
            iter2 = static_cast<uint8_t*>(img2.getData());
            for (size_t i = 0; i < n; ++i, ++iter, ++iter2)
                *iter2 *= *iter;
            t.toc("LOOP mult by array");

            t.tic();
            img3 *= img;
            t.toc("IMAGE mult by array");
            ASSERT_EQ(img2, img3);

            std::cout << std::endl;

            //------------------ Check Multiplication by VALUE ---------------
            img3 = img2 = img;

            t.tic();
            iter2 = static_cast<uint8_t*>(img2.getData());
            for (size_t i = 0; i < n; ++i, ++iter, ++iter2)
            *iter2 *= 100;
            t.toc("LOOP mult by value");

            t.tic();
            img3 *= 100;
            t.toc("IMAGE mult by value");

            ASSERT_EQ(img2, img3);

            std::cout << std::endl;

        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping image processing tests, EM_TEST_DATA not "
                     "defined in environment. " << std::endl;
    }
} // TEST Image.Performance
