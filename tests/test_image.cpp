//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

#include "emc/base/error.h"
#include "emc/base/image.h"
#include "emc/base/timer.h"

#include "test_common.h"

using namespace emcore;


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


// ===================== ImageFile TESTS =======================

TEST(ImageFile, Static)
{
    ASSERT_TRUE(ImageFile::hasImpl("spi"));
    ASSERT_TRUE(ImageFile::hasImpl("spider"));

    ASSERT_TRUE(ImageFile::hasImpl("mrc"));
    ASSERT_TRUE(ImageFile::hasImpl("mrcs"));

    ASSERT_TRUE(ImageFile::hasImpl("img"));
    ASSERT_TRUE(ImageFile::hasImpl("hed"));

    auto formatTypes = ImageFile::getFormatTypes();
    ASSERT_EQ(ImageFile::getImplTypes("spider"), formatTypes["spider"]);
    ASSERT_EQ(ImageFile::getImplTypes("mrc"), formatTypes["mrc"]);
    ASSERT_EQ(ImageFile::getImplTypes("img"), formatTypes["imagic"]);

} // TEST(ImageFile, Impl)

TEST(ImageFile, Basic)
{
    const size_t DIM = 16; // 128
    auto adim = ArrayDim(DIM, DIM, 1, 10);

    Image image;
    // Create an empty ImageFile
    ImageFile imageFile;
    // Most methods call should throw an Error in this state
    EXPECT_THROW(imageFile.getDim(), Error);
    EXPECT_THROW(imageFile.getType(), Error);

    // Let's open a file for writing
    imageFile.open("test.mrcs", File::Mode::TRUNCATE);
    // Now there is an internal implementation (mrc)
    // but still there is no type and default dimensions
    ASSERT_TRUE(imageFile.getType().isNull());
    ASSERT_EQ(imageFile.getDim(), ArrayDim());

    // We can't read from a newly opened file
    EXPECT_THROW(imageFile.read(1, image), Error);
    // We can't expand a newly created file without type
    EXPECT_THROW(imageFile.expand(10), Error);
    // One option is to use createEmpty, but it will
    // thrown an Exception if type is not supported
    EXPECT_THROW(imageFile.createEmpty(adim, typeInt32), Error);
    // Let's use float, that is supported by MRC
    EXPECT_NO_THROW(imageFile.createEmpty(adim, typeFloat));

    StringVector exts = {"mrc", "spi", "img"};

    for (auto ext: exts)
    {
        ImageFile imageFile;
        std::cout << "Using IO: " << ext << std::endl;
        std::string fn;
        // Write a single image
        fn = "image-single." + ext;
        imageFile.open(fn.c_str(), File::Mode::TRUNCATE);
        imageFile.createEmpty(ArrayDim(DIM, DIM, 1, 1), typeFloat);
        imageFile.close();

        // Write a stack of images
        fn = "image-stack." + ext;
        imageFile.open(fn.c_str(), File::Mode::TRUNCATE);
        imageFile.createEmpty(ArrayDim(DIM, DIM, 1, 100), typeFloat);

        Image img(ArrayDim(DIM, DIM, 1, 1), typeFloat);
        auto av = img.getView<float>();
        av.assign(200);
        imageFile.write(1, img);
        imageFile.close();
    }
} // TEST(ImageFile, Create)


TEST(MrcFile, Read)
{
    ImageFile mrcIO = ImageFile();
    // ASSERT_EQ(mrcIO.getName(), "mrc");

    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    auto td = TestData();

    fileDims["xmipp_tutorial/micrographs/BPV_1386.mrc"] = ArrayDim(9216, 9441, 1, 1);
    std::string stackFn = "emx/alignment/Test2/stack2D.mrc";
    ArrayDim stackDim(128, 128, 1, 100);
    // fileDims[stackFn] = stackDim;

    for (auto &pair: fileDims)
    {
        Image img;
        loc.index = 1;
        loc.path = td.get(pair.first);
        std::cout << ">>> Reading image: " << loc << std::endl;

        img.read(loc);
        std::cout << img << std::endl;
        ArrayDim imgDim(pair.second);
        imgDim.n = 1;
        ASSERT_EQ(img.getDim(), imgDim);
    }

    // Use mrcIO2 for writing individual images
    ImageFile mrcIO2 = ImageFile();
    // Use mrcIO3 for write another stack
    ImageFile mrcIO3 = ImageFile();

    mrcIO.open(td.get(stackFn));
    Image img;
    char suffix[4];
    std::string imgFn;
    ArrayDim imgDim(stackDim);
    imgDim.n = 1;

    ArrayDim outDim(stackDim);
    outDim.n = 10;
    std::string outputStackFn = "image_stack.mrcs";
    mrcIO3.open(outputStackFn,  File::Mode::TRUNCATE);
    mrcIO3.createEmpty(outDim, mrcIO.getType());

    for (size_t i = 1; i <= outDim.n; ++i)
    {
        mrcIO.read(i, img);
        snprintf (suffix, 4, "%03d", (int)i);
        imgFn = std::string("image") + suffix + ".mrc";
        std::cout << ">>> Writing image: " << imgFn << std::endl;
        mrcIO2.open(imgFn,  File::Mode::TRUNCATE);
        mrcIO2.write(1, img);
        mrcIO2.close();

        // Write to stack
        mrcIO3.write(i, img);
    }
    mrcIO.close();
    mrcIO3.close();

} // TEST(ImageMrcIO, Read)


TEST(ImageFile, WriteStack)
{
    auto td = TestData();
    std::string inputFn = td.get("relion_tutorial/micrographs/*.mrc");
    Glob glob(inputFn);

    std::cout << "DEBUG: input pattern: " << inputFn << std::endl
              << "         input files: " << glob.getSize() << std::endl;

    Image imageIn, imageOut;
    ImageFile input, output;
    output.open("mics.mrcs", File::TRUNCATE);

    for (size_t i = 0; i<glob.getSize(); ++i)
    {
        input.open(glob.getResult(i));
        input.read(1, imageIn);
        if (i == 0)
        {
            auto adim = imageIn.getDim();
            imageOut.resize(adim, typeInt8);
            adim.n = 2; // glob.getSize();
            output.createEmpty(adim, typeInt8);
        }
        imageOut.copy(imageIn);
        output.write(i+1, imageOut);
        input.close();
    }
    output.close();
}

TEST(SpiderImageFile, Read)
{
    ASSERT_TRUE(ImageFile::hasImpl("spider"));
    ImageFile spiIO = ImageFile();
    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    auto td = TestData();

    fileDims["emx/alignment/testAngles/sphere_128.vol"] = ArrayDim(128, 128, 128, 1);
    fileDims["emx/alignment/testAngles/proj.spi"] = ArrayDim(128, 128, 1, 1);
    fileDims["emx/alignment/testAngles/projections.stk"] = ArrayDim(128, 128, 1, 5);

    for (auto &pair: fileDims)
    {
        Image img;
        loc.index = 1;
        std::cout << ">>> Reading " << pair.first << std::endl;
        loc.path = td.get(pair.first);
        img.read(loc);
        std::cout << "Back in test" << std::endl;
        std::cout << img << std::endl;
        ArrayDim imgDim(pair.second);
        imgDim.n = 1;
        ASSERT_EQ(img.getDim(), imgDim);
    }
} // TEST(ImageSpiderIO, Read)

TEST(SpiderImageFile, Write)
{
    ASSERT_TRUE(ImageFile::hasImpl("spider"));

    auto td = TestData();
    ImageFile imageFile;
    auto stkFn = td.get("groel/classes/level_classes.stk");
    ImageLocation loc(stkFn, 1);
    auto imgDim = ArrayDim(140, 140, 1, 1);

    // Let's read the first image from the stack and write as single image
    Image image;
    image.read(loc);
    ASSERT_EQ(image.getDim(), imgDim);
    loc.path = "class-1.spi";
    image.write(loc);

    // Let's now open the single image and try to write to it more images
    imageFile.open(loc.path, File::Mode::READ_WRITE);
    // After the open, the dims should be the same as the image, but with n=2
    imgDim.n = 2;
    ASSERT_EQ(imageFile.getDim(), imgDim);
    //EXPECT_THROW(imageFile.write(2, image), Error);
    imageFile.close();

    // Let's now open a new file to write the entire stack
    imageFile.open(stkFn);
    auto stkDim = imageFile.getDim();
    std::string stkFnOut("classes.stk");
    ImageFile imageFileOut(stkFnOut, File::Mode::TRUNCATE);

    // Spider does not support any type other than float
    EXPECT_THROW(imageFileOut.createEmpty(stkDim, typeDouble), Error);
    // The previous call should succeed with typeFloat
    imageFileOut.createEmpty(stkDim, typeFloat);

    for (size_t i = 1; i <= stkDim.n; ++i)
    {
        imageFile.read(i, image);
        imageFileOut.write(i, image);
    }
    imageFileOut.close();

    // Let's try to expand the stack to double the size
    imageFileOut.open(stkFnOut, File::Mode::READ_WRITE);
    imageFileOut.expand(stkDim.n * 2);

//    for (size_t i = 1; i <= stkDim.n; ++i)
//    {
//        imageFile.read(i, image);
//        imageFileOut.write(i+stkDim.n, image);
//    }
    imageFileOut.close();

    imageFileOut.open(stkFnOut);
    ASSERT_EQ(imageFileOut.getDim().n, stkDim.n*2);
    imageFileOut.close();

    imageFile.close();

} // TEST(ImageSpiderIO, Write)

//TEST(ImageIOPng, Read)
//{
//    ImageFile pngIO = ImageFile();
//    auto td = TestData();
//    ImageLocation loc;
//    loc.path = td.get("8bits.png");
//    loc.index = 1;
//    Image img;
//    img.read(loc);
//    std::cout << ">>> Image: " << img;
//
//    auto path = "8bits-copy.png";
//
//    std::cout << ">>> Writing image: " << path << std::endl;
//
//    pngIO.open(path, File::Mode::TRUNCATE);
//    pngIO.createEmpty(img.getDim(), img.getType());
//    pngIO.write(1, img);
//    pngIO.close();
//
//    std::cout << ">>> Write image done." << std::endl;
//}
//
//TEST(ImageIOJpeg, Read)
//{
//    ImageFile jpegIO = ImageFile();
//    auto td = TestData();
//
//    ImageLocation loc;
//    loc.path = td.get("1-GRAY-8bits.jpg");
//    loc.index = 1;
//    Image img;
//    img.read(loc);
//    std::cout << ">>> Image: " << img;
//
//    auto path = "1-GRAY-8bits-copy.jpg";
//
//    std::cout << ">>> Writing image: " << path << std::endl;
//
//    jpegIO.open(path, File::Mode::TRUNCATE);
//    jpegIO.createEmpty(img.getDim(), img.getType());
//    jpegIO.write(1, img);
//    jpegIO.close();
//
//    std::cout << ">>> Write image done." << std::endl;
//}

TEST(ImageIOImagic, Read)
{
    ImageFile imagicIO = ImageFile();
    auto td = TestData();
    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;

    fileDims["xmipp_tutorial/particles/BPV_1386_ptcls.img"] = ArrayDim(500, 500, 1, 29);

    char suffix[4];
    std::string imgFn;
    ArrayDim imgDim(500, 500, 1, 1);

    for (auto &pair: fileDims)
    {
        for (size_t index = 1; index <= pair.second.n; index++)
        {
            Image img;
            loc.index = index;
            loc.path = td.get(pair.first);
            std::cout << ">>> Reading image: " << loc << std::endl;
            img.read(loc);
            std::cout << ">>> Image: " << img;
            //write
            snprintf (suffix, 4, "%03d", (int)index);
            imgFn = std::string("image") + suffix + ".hed";
            std::cout << ">>> Writing image: " << imgFn << std::endl;

            imagicIO.open(imgFn, File::Mode::TRUNCATE);
            imagicIO.createEmpty(imgDim, img.getType());
            imagicIO.write(1, img);
            imagicIO.close();
        }
    }
} // TEST(ImageMrcIO, Read)


// ===================== Image TESTS =======================

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


TEST(Image, Performance)
{
    auto td = TestData();

    // Load a big ~ 10k x 10k to check the performance of some
    // operations
    std::string micFn = td.get("xmipp_tutorial/micrographs/BPV_1386.mrc");

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
    array->set(100);
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
} // TEST Image.Performance
