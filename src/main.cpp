#include <iostream>
#include <complex>
#include <cassert>

#include "emc/base/type.h"
#include "emc/base/object.h"
#include "emc/base/array.h"
#include "emc/base/timer.h"

using namespace emcore;


int main()
{
    std::cout << "Hello, World!" << std::endl;

    Timer t;

    t.tic();

    Object o = 1;
    o = 2;

    int x = o;

    std::cout << "x = " << x << std::endl;

    Object o2 = 3.5;

    o2 = 1.3f;

    float d = o2;

    std::cout << "d = " << d << std::endl;

    o2 = 5.6f;

    float d2 = 5.6 + float(o2);
    float d3;

    std::cout << "d2 = " << d2 << std::endl;

    std::cout << "T0: " << *Type::get<Object>() << std::endl;

    Type * type1 = Type::get<double>();
    o2 = 1.1; // o2 should be float
    assert(o2.type() == type1);
    std::cout << "T1: " << *type1 << std::endl;

    Type * type2 = Type::get<float>();
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

    t.toc(">>> Loop 1: ");

    t.tic();

    for (int i = 0; i < N; i++)
    {
        d = values[i % 6];
        d /= 5.f;
        d2 = 500.6 + d;
        d3 = d2 / 100.f;
    }

    t.toc(">>> Loop 2: ");

    std::cout << "Creating an Array: " << std::endl;

    ArrayDim adim(10, 10);
    Array<int> A(adim);
    A.assign(11);
    A(3, 3) = 20;
    A(4, 4) = 20;
    A(5, 5) = 20;
    int * ptr = A.getData<int>();
    ptr[10] = 15;

    std::cout << A.toString() << std::endl;

    Array<float> A2(adim);

    return 0;

}