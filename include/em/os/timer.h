//
// Created by josem on 12/26/16.
//

#ifndef EM_CORE_TIMER_H
#define EM_CORE_TIMER_H

class TimerImpl;

namespace em
{
    /** @ingroup os
     * This class will encapsulate the logic for time printing.
     * Useful for debugging.
     */

    class Timer
    {
    public:
        Timer();
        size_t now();
        size_t tic();
        size_t toc(const char * msg=NULL, bool inSecs=true);
        size_t elapsed();

    private:
        TimerImpl *pimpl;
    };

} // namespace em


#endif //EM_CORE_TIMER_H
