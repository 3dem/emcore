//
// Created by josem on 3/12/18.
//

#ifndef EM_CORE_FUNCTIONS_H
#define EM_CORE_FUNCTIONS_H

namespace em
{
/** Class representing a 1D or 2D gaussian function at a given point.
 *
 * For the 1D case the operator() will return the value of a univariate
 * gaussian function at the point x.
 *
 * For the 2D case operator() will return the value of a multivariate (2D)
 * gaussian function at the point (x,y) when the X axis of the gaussian is
 * rotated ang (counter-clockwise) radians (the angle is positive when measured
 * from the universal X to the gaussian X).
 * X and Y are supposed to be independent.
 */
    template<class T=double>
    class GaussianFunc
    {
    public:
        /** Constructor for 1D. Mean and Standard deviation should be provided. */
        GaussianFunc(T stddevX, T meanX = 0);

        /** Constructor for 2D. */
        GaussianFunc(T stddevX, T stddevY, T angle, T meanX = 0, T meanY = 0);

        T operator()(T x);

        T operator()(T x, T y);

    private:
        T stddevX, meanX, stddevY, meanY, angle;
        T ca, sa, ps; // Values to pre-compute some operations
    }; // class GaussianFunc

} // namespace em

/** 1D gaussian value
 *
 * This function returns the value of a univariate gaussian function at the
 * point x.
 */
template <class RFLOAT>
RFLOAT gaussian1D(RFLOAT x, RFLOAT sigma, RFLOAT mu = 0);

/** 2D gaussian value
 *
 * This function returns the value of a multivariate (2D) gaussian function at
 * the point (x,y) when the X axis of the gaussian is rotated ang
 * (counter-clockwise) radians (the angle is positive when measured from the
 * universal X to the gaussian X). X and Y are supposed to be independent.
 */
 template <class RFLOAT>
RFLOAT gaussian2D(RFLOAT x,
                  RFLOAT y,
                  RFLOAT sigmaX,
                  RFLOAT sigmaY,
                  RFLOAT ang,
                  RFLOAT muX = 0,
                  RFLOAT muY = 0);


#endif //EM_CORE_FUNCTIONS_H