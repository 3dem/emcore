//
// Created by josem on 3/3/18.
//

#ifndef EM_CORE_FFT_H
#define EM_CORE_FFT_H

#include "em/image/image.h"


namespace em
{

    /** Enum class to hold the direction of the Fourier Transform */
    enum class FT
    {
        FORWARD,
        BACKWARD
    }; // enum class FT

    /** Wrapper class around the FFTW functions for Fourier transforms.
     * @ingroup proc
     * The memory for the Fourier transform is handled internally by this class.
     */
    class FourierTransformer
    {
    public:
        /** Default Ctor */
        FourierTransformer();
        /** Initialize the transformer with a pair of images */
        //FourierTransformer(Image &rImg, Image &fImg);

        ~FourierTransformer();

        /** Change the dimensions of the current Array.
         * This operation usually imply a new allocation of memory.
         * Optionally, a new type can be passed.
         */
        //virtual void resize(const ArrayDim &adim, const Type & type=typeNull);
        //void transform(FT direction=FT::FORWARD);

        /** TODO: Document!!! */
        void forward(Image &rImg, Image &fImg);

        /** TODO: Document!!! */
        void backward(Image &fImg, Image &rImg);

        class Impl; // Internal implementation class (PIMPL idiom)
        Impl * impl;
    }; // class FourierTransformer

} // namespace em


#endif //EM_CORE_FFT_H
