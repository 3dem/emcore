//
// Created by josem on 9/28/17.
//

//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/os/file.h"


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

    ASSERT_EQ(File::getSize(filename), 4*1024);

    File::expand(f, 5*1024);
    fclose(f);

    ASSERT_EQ(File::exists(filename), true);
    ASSERT_EQ(File::getSize(filename), 5*1024);

    ASSERT_EQ(remove(filename), 0);
}


