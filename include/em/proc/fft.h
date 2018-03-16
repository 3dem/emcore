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
        void forward(const Image &rImg, Image &fImg);

        /** TODO: Document!!! */
        void backward(Image &fImg, Image &rImg);

        /** Shift zero-frequency component to center of spectrum if the
         * direction is FT::FORWARD.
         * After applying this operation, the the zero-frequency component
         * of the input Fourier transform will by shifted to the center of
         * the array.
         *
         * If y = 1, swaps the left and right halves of X (single row)
         * If z = 1, swaps the first quadrant with the third,
         *           and the second quadrant with the fourth.
         * If z > 1, swaps half-spaces of X along each dimension.
         *
         * This operation is similar to Matlab fftshift.
         *
         * If the direction is FT::BACKWARD, the modification done by
         * a forward center will be undone, similar to Matlab ifftshift
         */
        void shift(const Image &fImgIn, Image &fImgOut,
                   FT direction = FT::FORWARD);
        void shift(Image &inOutImg, FT direction = FT::FORWARD);

        /** Change the dimensions of an image resulting from a forward
         * transform. The dimensions can be reduced or augmented.
         */
        void windowFT(const Image &fImgIn, Image &fImgOut, size_t newdim);
        void windowFT(Image &inOutImg, size_t newdim);

        /** Scale an image in real space by computing its FT, apply a window
         * in Fourier space and transforming back to real space.
         * @param inputImg Input image in real space.
         * @param outputImg Output image in real space with new dimension.
         * @param newdim New dimension of the output image
         */
        void scale(const Image &inputImg, Image &outputImg, size_t newdim);
        void scale(Image &inOutImg, size_t newdim);

        /** Return the corresponding dimensions in Fourier space given
         * the input dimensions in real space. The new x dimension will
         * be x / 2 + 1
         * @param rDim Input dimension in real space
         * @return new ArrayDim where nx = x / 2 + 1
         */
        static ArrayDim getDimFT(const ArrayDim &rDim);


        class Impl; // Internal implementation class (PIMPL idiom)
        Impl * impl;
    }; // class FourierTransformer

} // namespace em


#endif //EM_CORE_FFT_H
