//
// Created by Jose Miguel de la Rosa Trevin on 2017-06-09.
//

#include <iostream>
#include "gtest/gtest.h"

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


    ASSERT_TRUE(Image::hasIO("spi"));
    ASSERT_TRUE(Image::hasIO("spider"));
    ImageIO * reader = Image::getIO("spi");
    ASSERT_EQ(reader->getName(), "spider");

    ASSERT_TRUE(Image::hasIO("mrc"));
    ImageIO * reader2 = Image::getIO("mrc");
    ASSERT_EQ(reader2->getName(), "mrc");

} // TEST(ArrayTest, Constructor)
