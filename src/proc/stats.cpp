//
// Created by josem on 11/7/17.
//

#include "em/proc/stats.h"

using namespace emcore;
namespace emc = emcore;

// -------------- Stats Implementation ---------------------------

template <typename T>
Stats computeStats(const T * data, size_t size, Stats::Operation op)
{
    Stats s;
    s.min = s.max = static_cast<double>(data[0]);
    s.std = s.mean = 0;
    auto iter = data;

    if (op == Stats::MIN_MAX)  // Only compute min and max
    {
        for (size_t i = 0; i < size; ++i, ++iter)
        {
            double v = static_cast<double>(*iter);
            if (v < s.min)
                s.min = v;
            else if (v > s.max)
                s.max = v;
        }
    }
    else
    {
        for (size_t i = 0; i < size; ++i)
        {
            double v = static_cast<double>(data[i]);
            if (v < s.min)
                s.min = v;
            else if (v > s.max)
                s.max = v;
            s.mean += v;
            s.std += v * v;
        }

        if (size > 1)
        {
            s.mean /= size;
            s.std = s.std / size - s.mean * s.mean;
            s.std *= size / (size - 1);
            // Foreseeing numerical instabilities
            s.std = sqrt(static_cast< double >(abs(s.std)));
        } else
            s.std = 0;
    }

    return s;
} // template function computeStats<T>


Stats Stats::compute(const Type &type, const void *memory, size_t n,
                     Operation op)
{
#define STATS_IF(T) if (type == Type::get<T>()) \
                       return computeStats(static_cast<const T*>(memory), n, op)
    STATS_IF(float);
    STATS_IF(double);
    STATS_IF(int8_t);
    STATS_IF(uint8_t);
    STATS_IF(int16_t);
    STATS_IF(uint16_t);
    STATS_IF(int32_t);
    STATS_IF(uint32_t);
    STATS_IF(int64_t);
    STATS_IF(uint64_t);
    THROW_ERROR(std::string("Stats can not be computed for type: ")
                + type.getName());
#undef STATS_IF
} // Stats.compute


Stats Stats::compute(const Array &array, Operation op)
{
    return compute(array.getType(), array.getData(), array.getDim().getSize(), op);
} // Stats::compute

std::ostream& emc::operator<< (std::ostream &ostrm, const Stats &s)
{
    ostrm << "min: " << s.min << " max: " << s.max
          << " avg: " << s.mean << " std: " << s.std << " ";
    return ostrm;
}