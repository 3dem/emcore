//
// Created by josem on 3/3/18.
//

#include <fftw3.h>

#include "em/proc/fft.h"
#include "em/base/array.h"

using namespace em;

/** Base underlying implementation for wrapping FFTW functions.
 * It will be subclasses to support float and double operations
 */
class FourierTransformer::Impl
{
protected:
    ArrayDim inputDim;
    int dims[4]; // Dimension array to use in FFTW functions

public:
    virtual void cleanup() = 0;
    virtual void createPlans() = 0;
    virtual void destroyPlans() = 0;

    virtual void transform(FT direction) = 0;
}; // class FourierTransformer::Impl


class FtFloatImpl: public FourierTransformer::Impl
{
private:

    bool hasPlans = false;  // True if the plans are set
    fftwf_plan plan, iplan; // Direct and inverse plans
    float * inputData;
    fftwf_complex * outputData;

public:
    virtual void cleanup() override { fftwf_cleanup(); }
    virtual void createPlans() override
    {
        destroyPlans();
        int rank = inputDim.getRank();
        plan = fftwf_plan_dft_r2c(rank, dims, inputData, outputData,
                              FFTW_ESTIMATE); // TODO: consider other flags??
        iplan = fftwf_plan_dft_c2r(rank, dims, outputData, inputData,
                              FFTW_ESTIMATE); // TODO: consider other flags??
    }

    virtual void destroyPlans() override
    {
        if (hasPlans)
        {
            fftwf_destroy_plan(plan);
            fftwf_destroy_plan(iplan);
        }
    }

    virtual void transform(FT direction) override
    {
        auto & p = (direction == FT::FORWARD) ? plan : iplan;
        fftwf_execute(p);
    }
}; // class FtFloatImpl


void FourierTransformer::transform(FT direction)
{
    impl->transform(direction);
}
