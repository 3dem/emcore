//
// Created by josem on 12/26/16.
//

#include <iostream>

#include "emc/base/timer.h"
#include "emc/base/error.h"

using namespace emcore;


class Timer::Impl
{
public:
    TimeStamp tic;
}; // class Timer::Impl

Timer::Timer() { impl = new Impl(); }

Timer::~Timer() { delete impl; }

Timer::TimeStamp Timer::now()
{
    return std::chrono::high_resolution_clock::now();
}

Timer::TimeStamp Timer::tic()
{
    // Assign and return 'now' value
    return impl->tic = now();
}

double Timer::toc(const char * msg, bool inSecs)
{
    auto diff = elapsed();

    if (msg != NULL)
        std::cout << msg;
    std::cout << " Elapsed time: ";

    if (inSecs)
        std::cout << diff.count() << " secs." << std::endl;
    else
        THROW_ERROR("Not implemented");

    return diff.count();
}

Timer::Duration Timer::elapsed()
{
    return now() - impl->tic;
}