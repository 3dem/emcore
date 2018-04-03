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
    virtual void normalize() = 0;

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


template <class T>
void _normalize(void * rawData, size_t n)
{
    std::cout << "Normalizing by: " << n << std::endl;
    auto data = static_cast<float*>(rawData);
    for (size_t i = 0; i < n; ++i, ++data)
        *data /= n;
} // function normalize<T>



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
        fftwf_execute(direction == FT::FORWARD ? plan : iplan);
    }

    virtual void normalize() override
    {
        _normalize<float>(inputData, inputDim.getSize());
    }
}; // class FtFloatImpl


class FtDoubleImpl: public FourierTransformer::Impl
{
private:
    bool hasPlans = false;  // True if the plans are set
    fftw_plan plan, iplan; // Direct and inverse plans

public:
    virtual void cleanup() override
    {
        fftw_cleanup();
    }

    virtual void createPlans() override
    {
        destroyPlans();
        int rank = inputDim.getRank();
        auto input = static_cast<double*>(inputData);
        auto output = static_cast<fftw_complex *>(outputData);
        // Set the dimensions to the dims array as expected by FFTW
        dims[0] = (int)inputDim.z;
        dims[1] = (int)inputDim.y;
        dims[2] = (int)inputDim.x;
        // The starting memory within dims will depends on the rank of the input
        int index = 3 - inputDim.getRank();
        plan = fftw_plan_dft_r2c(rank, dims + index, input, output,
                                  FFTW_ESTIMATE); // TODO: consider other flags??
        iplan = fftw_plan_dft_c2r(rank, dims + index, output, input,
                                   FFTW_ESTIMATE); // TODO: consider other flags??
    }

    virtual void destroyPlans() override
    {
        if (hasPlans)
        {
            fftw_destroy_plan(plan);
            fftw_destroy_plan(iplan);
        }
    }

    virtual void transform(FT direction) override
    {
        fftw_execute(direction == FT::FORWARD ? plan : iplan);
    }

    virtual void normalize() override
    {
        _normalize<double>(inputData, inputDim.getSize());
    }
}; // class FtDoubleImpl

FourierTransformer::FourierTransformer()
{
    impl = nullptr;
}

FourierTransformer::~FourierTransformer()
{
    if (impl != nullptr)
        impl->destroyPlans();
    delete impl;
}

void FourierTransformer::forward(const Image &rImg, Image &fImg)
{
    std::cout << "FourierTransformer::forward" << std::endl
              << "     rMem: " << rImg.getData() << std::endl
              << "     fMem: " << fImg.getData() << std::endl;

    if (impl == nullptr)
        impl = new FtFloatImpl();

    impl->setImages(rImg, fImg);
    impl->transform(FT::FORWARD);
} // function FourierTransform.forward

void FourierTransformer::backward(Image &fImg, Image &rImg)
{
    if (impl == nullptr)
        impl = new FtFloatImpl();

    std::cout << "FourierTransformer::backward" << std::endl
              << "     rMem: " << rImg.getData() << std::endl
              << "     fMem: " << fImg.getData() << std::endl;
    impl->setImages(rImg, fImg);
    impl->transform(FT::BACKWARD);
    impl->normalize();
} // function FourierTransformer.backward

void FourierTransformer::shift(const Image &fImgIn, Image &fImgOut,
                               FT direction)
{
    // FIXME: The assignment does not seem to resize the image
    fImgOut = fImgIn;
    shift(fImgOut, direction); // TODO: Shift could be more efficient if not inplace
} // function FourierTransformer.shift

void FourierTransformer::shift(Image &inOutImg, FT direction)
{
    auto adim = inOutImg.getDim();
    auto rank = adim.getRank();
    auto type = inOutImg.getType();
    auto size = type.getSize();

    std::cout << "adim: " << adim << " rank: " << rank << " type: " << type << " size: " << size << std::endl;

    bool isForward = direction == FT::FORWARD;
    size_t n = adim.x; // assuming all dimensions are the same
    size_t r = n % 2;
    size_t h = n / 2;

    // Left memory position is always 0, let's calculate
    // the left size and right pos and size depending on the
    // direction and also if the input dim is even or odd
    size_t lIndex = isForward ? h + r : h;  // number of items in first half
    size_t lJump = isForward ? lIndex - r: lIndex + r;
    size_t rJump = lJump - n;  // assuming all dimensions are the same
    size_t lSize =  lIndex * size;
    size_t ldPos = lJump * size;
    size_t rPos = (isForward ? h + r : h) * size;
    size_t rSize = (isForward ? h : h + r) * size;


#define SWAP(mem1, mem2, buffer, msize) { memcpy(buffer, mem1, msize); \
                                          memcpy(mem1, mem2, msize); \
                                          memcpy(mem2, buffer, msize); }

#define SWAPX(mem) { memcpy(buffer, mem, lSize); \
                     memcpy(mem, mem+rPos, rSize); \
                     memcpy(mem+ldPos, buffer, lSize); }

    uint8_t * buffer = nullptr;
    auto data = static_cast<uint8_t *>(inOutImg.getData());

    switch (rank)
    {
        case 1:
        {
            // Half a row size to do shift in X
            buffer = new uint8_t[(h + r) * size];
            SWAPX(data);
            break;
        }
        case 2:
        {
            size_t rowSize = n * size;
            buffer = new uint8_t[rowSize];

            auto rowData = data;
            auto otherRow = data;

            size_t rowHalfJump = rowSize * h;

            // First of all, swap X all rows
            for (size_t row = 0; row < n; ++row, rowData += rowSize)
                SWAPX(rowData)

            rowData = data;
            // If r = 0, we can do a more direct swap, if not
            // we need to jump to the next position
            if (r == 0)
            {
                // Example: n = 4 (forward)
                // 0 <-> 2,  1 <-> 3
                for (size_t row = 0; row < h; ++row, rowData += rowSize)
                {
                    otherRow = rowData + rowHalfJump;
                    SWAP(rowData, otherRow, buffer, rowSize);
                }
            }
            else
            {
                // Example: n = 5 (forward)
                // 0 -> 2,  2 -> 4,   4 -> 1,   1 -> 3,  3 -> 0
                // Copy first row to the buffer
                memcpy(buffer, data, rowSize);
                for (size_t i = 0, index = 0; i < n; ++i)
                {
                    index += index < lIndex ? lJump : rJump;
                    // Now swap it to the next position (this will store
                    // the next position in buffer for next iter)
                    // Using the first row as a new 'buffer'
                    if (index == 0) // for the last index we just need to copy
                        memcpy(data, buffer, rowSize);
                    else
                        SWAP(buffer, data + index * rowSize, data, rowSize);
                }
            }
            break;
        }

        case 3:
            break;
    } // switch rank

    delete buffer;
} // function FourierTransformer.shift


template <class T>
void _windowFT(const Image &fImgIn, Image &fImgOut)
{
    auto idim = fImgIn.getDim();
    auto odim = fImgOut.getDim();

    LegacyArray<std::complex<T>> in(idim, const_cast<void *>(fImgIn.getData()));
    LegacyArray<std::complex<T>> out(odim, fImgOut.getData());

    if (fImgOut.getDim().x > fImgIn.getDim().x)
    {
        long int max_r2 = (XSIZE(in) -1) * (XSIZE(in) - 1);
        FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(in)
        {
            // Make sure windowed FT has nothing in the corners, otherwise we end up with an asymmetric FT!
            if (kp*kp + ip*ip + jp*jp <= max_r2)
                FFTW_ELEM(out, kp, ip, jp) = FFTW_ELEM(in, kp, ip, jp);
        }
    }
    else
    {
        FOR_ALL_ELEMENTS_IN_FFTW_TRANSFORM(out)
        {
            FFTW_ELEM(out, kp, ip, jp) = FFTW_ELEM(in, kp, ip, jp);
        }
    }
} // helper function _windowFT

void FourierTransformer::windowFT(const Image &fImgIn, Image &fImgOut, size_t newdim)
{
    auto idim = fImgIn.getDim();
    auto& type = fImgIn.getType();
    size_t &y = idim.y;

    ASSERT_ERROR(idim.getRank() != 2, "Only implemented for rank=2");

    size_t newhdim = newdim / 2 + 1;

    auto odim = ArrayDim(newhdim, newdim);

    ASSERT_ERROR(y > 1 && y / 2 + 1 != idim.x,
                 "Fourier Transform should be an image of equal sizes in "
                 "all dimensions.")

    fImgOut.resize(odim, type);
    if (type == typeCDouble)
        _windowFT<double>(fImgIn, fImgOut);
    else if (type == typeCFloat)
        _windowFT<float>(fImgIn, fImgOut);
    else
        THROW_ERROR("Only FT of CFloat or CDouble are allowed.");
} // function FourierTransformer.window

void FourierTransformer::windowFT(Image &fImgInOut, size_t newdim)
{
    Image tmp(fImgInOut);
    windowFT(tmp, fImgInOut, newdim);
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