//
// Created by josem on 3/3/18.
//

#ifndef EM_CORE_FFT_H
#define EM_CORE_FFT_H

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
        /** Default empty Ctor */
        FourierTransformer();

        void transform(FT direction=FT::FORWARD);

        class Impl; // Internal implementation class (PIMPL idiom)
        Impl * impl;
    }; // class FourierTransformer

} // namespace em


#endif //EM_CORE_FFT_H
