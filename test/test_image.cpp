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
    //header["filename"] = std::string("/path/to/image/");
    std::cout << img << std::endl;


    ASSERT_TRUE(Image::hasReader("spi"));
    ASSERT_TRUE(Image::hasReader("spider"));
    ImageReader * reader = Image::getReader("spi");
    ASSERT_EQ(reader->getName(), "spider");

} // TEST(ArrayTest, Constructor)
