//
// Created by josem on 12/26/16.
//

#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>

#include "em/os/timer.h"

using namespace em;


class TimerImpl
{
public:
    struct timeval tv;
    struct tm tm;
    size_t tic_time;
};

Timer::Timer()
{
    this->pimpl = new TimerImpl();
}

size_t Timer::now()
{
    gettimeofday(&pimpl->tv, NULL);
    localtime_r(&pimpl->tv.tv_sec, &pimpl->tm);
    return pimpl->tm.tm_hour * 3600 * 1000 + pimpl->tm.tm_min * 60 * 1000 +
            pimpl->tm.tm_sec * 1000 + pimpl->tv.tv_usec / 1000;
}

size_t Timer::tic()
{
    // Assign and return 'now' value
    return pimpl->tic_time = now();
}

size_t Timer::toc(const char * msg, bool inSecs)
{
    size_t diff = elapsed();

    if (msg != NULL)
        std::cout << msg;
    std::cout << " Elapsed time: ";

    if (inSecs)
        std::cout << diff/1000.0 << " secs." << std::endl;
    else
        std::cout << diff << " msecs." << std::endl;

    return diff;
}

size_t Timer::elapsed()
{
    return now() - pimpl->tic_time;
}