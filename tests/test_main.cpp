//
// Created by Jose Miguel de la Rosa Trevin on 2017-03-24.
//

#include <iostream>
#include "gtest/gtest.h"

#include "em/base/object.h"
#include "em/base/array.h"
#include "em/base/timer.h"

using namespace em;


// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(Main, ObjectLoop) {
    std::cout << std::endl << "Hello, World!" << std::endl;

    em::Timer t;

    float d = 1;
    Object o2(d);
    ASSERT_EQ(o2.getType(), typeFloat);

    float d2 = 5.6 + float(o2);
    float d3;

    std::cout << "d2 = " << d2 << std::endl;

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

    ArrayDim adim(10, 10);
    Array A(adim, em::typeInt32);
    ArrayView<int> Av = A.getView<int>();

    Av.assign(11);
    Av(3, 3) = 20;
    Av(4, 4) = 20;
    Av(5, 5) = 20;
    int * ptr = Av.getData();
    ptr[10] = 15;

    //std::cout << Av.toString() << std::endl;

    em::Array A2 = A;
//
//    ArrayView<int> Av2 = A2.getView<int>();
//
//    std::cout << Av2.toString() << std::endl;



}
