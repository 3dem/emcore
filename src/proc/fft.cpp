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
    Type inputType;
    int dims[4]; // Dimension array to use in FFTW functions
    // Hold pointers to input/output memory locations
    void * inputData = nullptr;
    void * outputData = nullptr;

public:
    virtual void cleanup() = 0;
    virtual void createPlans() = 0;
    virtual void destroyPlans() = 0;

    virtual void transform(FT direction) = 0;

    /** Set the images that will be used for the transform. */
    void setImages(Image &rImg, Image &fImg)
    {
        auto dim = rImg.getDim();
        auto dimOut(dim);
        dimOut.x = int(dim.x / 2) + 1;

        if (dimOut != fImg.getDim())
        {
            Type typeIn = rImg.getType();
            Type typeOut;

            if (typeIn == typeFloat)
                typeOut = typeCFloat;
            else if (typeIn == typeDouble)
                typeOut = typeCDouble;
            else
                THROW_ERROR(std::string("Unsupport FFT type: ")
                            + typeIn.getName());

            fImg.resize(dimOut, typeOut);
        }

        if (dim != inputDim || rImg.getData() != inputData ||
            fImg.getData() != outputData)
        {
            std::cout << "DEBUG_JM: Something has changed, r"
                         "e-calculating the plans..." << std::endl;

            inputData = rImg.getData();
            outputData = fImg.getData();
            inputDim = dim;
            createPlans();
        }
    }

    virtual ~Impl() {};
}; // class FourierTransformer::Impl


class FtFloatImpl: public FourierTransformer::Impl
{
private:
    bool hasPlans = false;  // True if the plans are set
    fftwf_plan plan, iplan; // Direct and inverse plans

public:
    virtual void cleanup() override
    {
        fftwf_cleanup();
    }

    virtual void createPlans() override
    {
        destroyPlans();
        int rank = inputDim.getRank();
        auto input = static_cast<float*>(inputData);
        auto output = static_cast<fftwf_complex *>(outputData);
        // Set the dimensions to the dims array as expected by FFTW
        dims[0] = (int)inputDim.z;
        dims[1] = (int)inputDim.y;
        dims[2] = (int)inputDim.x;
        // The starting memory within dims will depends on the rank of the input
        int index = 3 - inputDim.getRank();
        plan = fftwf_plan_dft_r2c(rank, dims + index, input, output,
                                  FFTW_ESTIMATE); // TODO: consider other flags??
        iplan = fftwf_plan_dft_c2r(rank, dims + index, output, input,
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

    virtual ~FtFloatImpl() {};
}; // class FtFloatImpl


FourierTransformer::FourierTransformer()
{
    impl = nullptr;
}

FourierTransformer::~FourierTransformer()
{
    delete impl;
}

//void FourierTransformer::forward(Image &rImg, Image &fImg)
//{
//    impl = new FtFloatImpl();
//    impl->setImages(rImg, fImg);
//}
//
//void FourierTransformer::transform(FT direction)
//{
//    impl->transform(direction);
//}

void FourierTransformer::forward(Image &rImg, Image &fImg)
{
    if (impl == nullptr)
        impl = new FtFloatImpl();

    impl->setImages(rImg, fImg);
    impl->transform(FT::FORWARD);
}

void FourierTransformer::backward(Image &fImg, Image &rImg)
{
    if (impl == nullptr)
        impl = new FtFloatImpl();

    impl->setImages(rImg, fImg);
    impl->transform(FT::BACKWARD);
}
