//
// Created by josem on 9/28/17.
//

//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/string.h"
#include "em/os/filesystem.h"


using namespace em;


TEST(File, Basic) {
    // Test basic properties of Type singleton instances
    const char * filename = "kk.binary";

    FILE *f = fopen(filename, "w");
    float v[1024];
    memset(v, 0, 1024*sizeof(float)); // Set all to zero

    // Write 1024 floats
    size_t written = fwrite(v, sizeof(float), 1024, f);
    std::cout << "Written: " << written << " elements" << std::endl;

    fflush(f);

    ASSERT_EQ(Path::getFileSize(filename), 4*1024);

    File::resize(f, 5 * 1024);
    fclose(f);

    ASSERT_EQ(Path::exists(filename), true);
    ASSERT_EQ(Path::getFileSize(filename), 5*1024);

    ASSERT_EQ(Path::remove(filename), 0);
    ASSERT_FALSE(Path::exists(filename));

    std::string filename2("non-existing");
    // Check that removing a non-existing file is fine
    ASSERT_EQ(Path::remove(filename2), 0);
} // TEST File.Basic


TEST(Path, Basic) {
    // Test basic properties of Type singleton instances
    std::string fn1 = "path/to.from/there/kk.binary";
    std::string dn1 = Path::getDirName(fn1);
    std::string bn1 = Path::getFileName(fn1);

    ASSERT_EQ(dn1, "path/to.from/there");
    ASSERT_EQ(Path::getDirName(dn1), "path/to.from");
    ASSERT_EQ(Path::getDirName(fn1 + "/"), fn1);

    ASSERT_EQ(bn1, "kk.binary");
    ASSERT_EQ(Path::getFileName(bn1), bn1);
    ASSERT_EQ(Path::getFileName(fn1 + "/"), "");

    ASSERT_EQ(Path::getExtension(fn1), "binary");
    ASSERT_EQ(Path::getExtension(bn1), "binary");
    ASSERT_EQ(Path::getExtension("binary"), "");

    ASSERT_EQ(Path::getExtension("a.b.c"), "c");
} // TEST Path.Basic


TEST(Glob, Basic) {
    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        std::string root(testDataPath);
        std::string p1 = root + "/xmipp_tutorial/micrographs/BPV_*.mrc";
        Glob g1(p1);
        std::string suffixes[] = {"1386", "1387", "1388"};

        ASSERT_EQ(g1.getSize(), 3);
        for (size_t i = 0; i < g1.getSize(); i++)
            ASSERT_EQ(g1.getResult(i), String::replace(p1, "*", suffixes[i]));

        for (size_t i = 0; i < g1.getSize(); i++)
            std::cout << g1.getResult(i) << std::endl;
    }
    else
    {
        std::cout << "Glob test can not be run: EM_TEST_DATA not defined in "
                     "environment. " << std::endl;
    }
}