//
// Created by josem on 7/20/19.
//

#ifndef EM_CORE_TEST_COMMON_H
#define EM_CORE_TEST_COMMON_H

#include "em/base/error.h"

using namespace em;


class TestData
{
private:
    std::string root;

public:
    TestData()
    {
        auto value = getenv("EM_TEST_DATA");

        if (value == nullptr)
            THROW_ERROR("EM_TEST_DATA variable is not defined.");

        root = value;
        if (!Path::exists(root))
            THROW_ERROR(std::string("EM_TEST_DATA path does not exist.") + root);
    }

    /** Return the path prefixed with test data root */
    std::string get(const std::string &path)
    {
        return root + "/" + path;
    }
}; // class TestData


#endif //EM_CORE_TEST_COMMON_H
