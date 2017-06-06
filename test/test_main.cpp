//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/object.h"
#include "em/base/array.h"
#include "em/os/timer.h"

using namespace em;


// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(IndependentMethod, ResetsToZero) {
    std::cout << std::endl << "Hello, World!" << std::endl;

    em::Timer t;

    t.tic();

    em::Object o = 1;
    o = 2;

    int x = o;

    std::cout << "x = " << x << std::endl;

    em::Object o2 = 3.5;

    o2 = 1.3f;

    float d = o2;

    std::cout << "d = " << d << std::endl;

    o2 = 5.6f;

    float d2 = 5.6 + float(o2);
    float d3;

    std::cout << "d2 = " << d2 << std::endl;

    std::cout << "T0: " << *em::Type::get<em::Object>() << std::endl;

    em::Type * type1 = em::Type::get<double>();
    o2 = 1.1; // o2 should be float
    assert(o2.type() == type1);
    std::cout << "T1: " << *type1 << std::endl;

    em::Type * type2 = em::Type::get<float>();
    std::cout << "T2: " << *type2 << std::endl;

    t.toc("Nothing");

    t.tic();

    size_t N = 10000000;
    float values [] = {1.5f, 2.3f, 5.7f, 3.2f, 10.f, 56.f};

    for (int i = 0; i < N; i++)
    {
        o2 = values[i % 6];
        d /= 5.f;
        d2 = 500.6 + float(o2);
        d3 = d2 / 100.f;
    }

    t.toc(">>> Loop 1 (using Object): ");

    t.tic();

    for (int i = 0; i < N; i++)
    {
        d = values[i % 6];
        d /= 5.f;
        d2 = 500.6 + d;
        d3 = d2 / 100.f;
    }

    t.toc(">>> Loop 2 (using float): ");

    std::cout << "Creating an Array: " << std::endl;

    em::ArrayDim adim(10, 10);
    em::Array<int> A(adim);
    A.assign(11);
    A(3, 3) = 20;
    A(4, 4) = 20;
    A(5, 5) = 20;
    int * ptr = A.getDataPointer<int>();
    ptr[10] = 15;

    std::cout << A.toString() << std::endl;

    em::Array<float> A2(adim);

}


// The fixture for testing class Project1. From google test primer.
class Project1Test : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    Project1Test() {
        // You can do set-up work for each test here.
    }

    virtual ~Project1Test() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Objects declared here can be used by all tests in the test case for Project1.

};

// Test case must be called the class above
// Also note: use TEST_F instead of TEST to access the test fixture (from google test primer)
TEST_F(Project1Test, MethodBarDoesAbc) {

}