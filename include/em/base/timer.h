//
// Created by josem on 12/26/16.
//

#ifndef EM_CORE_TIMER_H
#define EM_CORE_TIMER_H

#include <chrono>

using namespace std;

namespace em
{
    /** @ingroup base
     * This class will encapsulate the logic for time printing.
     * Useful for debugging.
     */

    class Timer
    {
    public:
        using TimeStamp = chrono::time_point<chrono::high_resolution_clock>;
        using Duration = chrono::duration<double>;

        Timer();
        ~Timer();
        TimeStamp now();
        TimeStamp tic();
        double toc(const char * msg=NULL, bool inSecs=true);
        Duration elapsed();

    private:
        class Impl;
        Impl * impl;
    };

} // namespace em


#endif //EM_CORE_TIMER_H
