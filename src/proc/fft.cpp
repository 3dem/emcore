//
// Created by josem on 3/3/18.
//

#include <fftw3.h>

#include "em/proc/fft.h"
#include "em/base/array.h"
#include "em/base/legacy.h"


using namespace em;


/** Base underlying implementation for wrapping FFTW functions.
 * It will be subclasses to support float and double operations
 */
class FourierTransformer::Impl
{
public:
    ArrayDim inputDim;
    Type inputType;
    int dims[4]; // Dimension array to use in FFTW functions
    // Hold pointers to input/output memory locations
    void * inputData = nullptr;
    void * outputData = nullptr;

    virtual void cleanup() = 0;
    virtual void createPlans() = 0;
    virtual void destroyPlans() = 0;

    virtual void transform(FT direction) = 0;

    /** Set the images that will be used for the transform. */
    void setImages(const Image &rImg, Image &fImg)
    {
        auto dim = rImg.getDim();
        // Computed the expected dimension of the Fourier image
        // from the given real space image dimensions
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
                THROW_ERROR(std::string("Unsupported FFT type: ")
                            + typeIn.getName());

            fImg.resize(dimOut, typeOut);
        }

        if (dim != inputDim || rImg.getData() != inputData ||
            fImg.getData() != outputData)
        {
            std::cout << "DEBUG_JM: Something has changed, r"
                         "e-calculating the plans..." << std::endl;

            // Remote the const-ness to store the input pointer to data
            // Believe me, we are not going to modify it (for now)
            inputData = const_cast<void*>(rImg.getData());
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

void FourierTransformer::forward(const Image &rImg, Image &fImg)
{
    std::cout << "FourierTransformer::forward" << std::endl
              << "     rMem: " << rImg.getData() << std::endl
              << "     fMem: " << fImg.getData() << std::endl;

    if (impl == nullptr)
        impl = new FtFloatImpl();

    impl->setImages(rImg, fImg);
    impl->transform(FT::FORWARD);
}

void FourierTransformer::backward(Image &fImg, Image &rImg)
{
    if (impl == nullptr)
        impl = new FtFloatImpl();

    std::cout << "FourierTransformer::backward" << std::endl
              << "     rMem: " << rImg.getData() << std::endl
              << "     fMem: " << fImg.getData() << std::endl;
    impl->setImages(rImg, fImg);
    impl->transform(FT::BACKWARD);
}

void FourierTransformer::windowFT(Image &fImgIn, Image &fImgOut, size_t newdim)
{
    auto idim = fImgIn.getDim();
    auto& type = fImgIn.getType();
    size_t &y = idim.y;

    ASSERT_ERROR(newdim > y, "Augmenting window NOT IMPLEMENTED yet.");
    ASSERT_ERROR(idim.getRank() != 2, "Only implemented for rank=2");

    size_t newhdim = newdim / 2 + 1;

    auto odim = ArrayDim(newhdim, newdim);

    ASSERT_ERROR(y > 1 && y / 2 + 1 != idim.x,
                 "Fourier Transform should be an image of equal sizes in "
                 "all dimensions.")

    fImgOut.resize(odim, type);

    auto memIn = static_cast<int8_t *>(fImgIn.getData());
    auto memOut = static_cast<int8_t *>(fImgOut.getData());
    size_t size = type.getSize();
    size_t jumpIn = newhdim * size;
    size_t jumpOut = idim.x * size;

    ASSERT_ERROR(type != typeCFloat, "Only Complex-Float type is implemented.")
    auto data = static_cast<float*>(fImgIn.getData());

    LegacyArray<std::complex<float>> in(idim, fImgIn.getData());
    LegacyArray<std::complex<float>> out(odim, fImgOut.getData());

    FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(out)
    {
        FFTW_ELEM(out, kp, ip, jp) = FFTW_ELEM(in, kp, ip, jp);
    }
} // function FourierTransformer.window

void FourierTransformer::scale(const Image &inputImg, Image &outputImg,
                               size_t newdim)
{
    Image fImg1, fImg2;
    forward(inputImg, fImg1);
    windowFT(fImg1, fImg2, newdim);
    ArrayDim odim(newdim, newdim);
    if (inputImg.getDim().getRank() == 3)
        odim.z = newdim;
    outputImg.resize(odim, inputImg.getType());
    backward(fImg2, outputImg);
} // function FourierTransformer.scale

void FourierTransformer::scale(Image &inOutImg, size_t newdim)
{
    scale(Image(inOutImg), inOutImg, newdim);
}  // function FourierTransformer.scale

ArrayDim FourierTransformer::getDimFT(const ArrayDim &rDim)
{
    return ArrayDim(rDim.x / 2 + 1, rDim.y, rDim.z, rDim.n);
}

//template<class T>
//void windowFourierTransform(MultidimArray<T > &in,
//                            MultidimArray<T > &out,
//                            long int newdim)
//{
//    long int newhdim = newdim/2 + 1;
//
//    // If same size, just return input
//    if (newhdim == XSIZE(in))
//    {
//        out = in;
//        return;
//    }
//
//    // Otherwise apply a windowing operation
//    // Initialise output array
//    switch (in.getDim())
//    {
//        case 1:
//            out.initZeros(newhdim);
//            break;
//        case 2:
//            out.initZeros(newdim, newhdim);
//            break;
//        case 3:
//            out.initZeros(newdim, newdim, newhdim);
//            break;
//        default:
//            REPORT_ERROR("windowFourierTransform ERROR: dimension should be 1, 2 or 3!");
//    }
//    if (newhdim > XSIZE(in))
//    {
//        long int max_r2 = (XSIZE(in) -1) * (XSIZE(in) - 1);
//        FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(in)
//        {
//            // Make sure windowed FT has nothing in the corners, otherwise we end up with an asymmetric FT!
//            if (kp*kp + ip*ip + jp*jp <= max_r2)
//                FFTW_ELEM(out, kp, ip, jp) = FFTW_ELEM(in, kp, ip, jp);
//        }
//    }
//    else
//    {
//        FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(out)
//        {
//            FFTW_ELEM(out, kp, ip, jp) = FFTW_ELEM(in, kp, ip, jp);
//        }
//    }
//}