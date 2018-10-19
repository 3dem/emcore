//
// Created by josem on 11/7/17.
//

#ifndef EM_CORE_STATS_H
#define EM_CORE_STATS_H

#include "em/base/image.h"


namespace em
{
    /** Computing min, max, avg and std on a given Array. */
    struct Stats
    {
        /** Enumerate possible operations to compute statistics.
         * Only computing MIN/MAX is faster.
         */
        enum Operation {ALL, MIN_MAX};

        /** Compute min, max, avg and std on the input array */
        static Stats compute(const Array& array,
                             Operation op=ALL);

        /** Compute min, max, avg and std on the input raw memory */
        static Stats compute(const Type& type, const void * memory, size_t n,
                             Operation op=ALL);

        double min, max, mean, std;
    };

    std::ostream& operator<< (std::ostream &ostrm, const Stats &s);

} // namespace em

#endif //EM_CORE_STATS_H
