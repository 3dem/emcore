//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/error.h"
#include "em/base/object.h"
#include "em/base/array.h"
#include "em/image/image.h"
#include "em/base/timer.h"


using namespace em;


TEST(Error, Basics) {

    std::string errorMsg = "Testing error";

    try
    {
        THROW_ERROR(errorMsg);
    }
    catch (Error &err)
    {
        ASSERT_EQ(err.msg, errorMsg);
        ASSERT_EQ(err.fileName, __FILE__);
        std::cout << err << std::endl;
    }

    try
    {
        FILE * pFile;
        pFile = fopen ("unexist.ent","r");
        if (pFile == NULL)
            THROW_SYS_ERROR("Error opening file unexist.ent");
    }
    catch (Error &err)
    {
        ASSERT_EQ(err.errorCode, ENOENT); // Not such file or directory error
        ASSERT_EQ(err.fileName, __FILE__);
        std::cout << err << std::endl;
    }
} // TEST(ArrayTest, Constructor)


TEST(Timer, Basics)
{

    Timer t;
    t.tic();

    for (int i=1; i < 1000000; ++i);

    t.toc();

} // TEST(ArrayTest, Constructor)
