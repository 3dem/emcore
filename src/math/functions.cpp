//
// Created by josem on 3/12/18.
//

#include <cmath>

#include "em/math/functions.h"


using namespace em;


template <class T>
GaussianFunc<T>::GaussianFunc(T stddevX, T meanX)
{
    this->stddevX = stddevX;
    this->meanX = meanX;

    ps = 1 / sqrt(2*M_PI*stddevX*stddevX);
}

template <class T>
GaussianFunc<T>::GaussianFunc(T stddevX, T stddevY, T angle, T meanX, T meanY)
{
    this->stddevX = stddevX;
    this->meanX = meanX;
    this->stddevY = stddevY;
    this->meanY = meanY;
    this->angle = angle;

    ps = 1 / sqrt(2*M_PI*stddevX*stddevY);
    ca = cos(angle);
    sa = sin(angle);
}

template <class T>
T GaussianFunc<T>::operator()(T x)
{
    x -= meanX;  // Express x in the gaussian internal coordinates
    x /= stddevX;
    return ps * exp(-0.5 * (x * x));
} // GaussianFunc<T>.operator(x)

template <class T>
T GaussianFunc<T>::operator()(T x, T y)
{
    // Express x,y in the gaussian internal coordinates
    x -= meanX;
    y -= meanY;
    T xp = (ca * x + sa * y) / stddevX;
    T yp = (-sa * x + ca * y) / stddevY;

    return ps * exp(-0.5 * (xp * xp + yp * yp));
} // GaussianFunc<T>.operator(x)


template <class RFLOAT>
RFLOAT gaussian1D(RFLOAT x, RFLOAT sigma, RFLOAT mu)
{
    x -= mu;
    return 1 / sqrt(2*M_PI*sigma*sigma)*exp(-0.5*((x / sigma)*(x / sigma)));
}

template <class RFLOAT>
RFLOAT gaussian2D(RFLOAT x, RFLOAT y, RFLOAT sigmaX, RFLOAT sigmaY,
                  RFLOAT ang, RFLOAT muX, RFLOAT muY)
{
    // Express x,y in the gaussian internal coordinates
    x -= muX;
    y -= muY;
    RFLOAT xp = cos(ang) * x + sin(ang) * y;
    RFLOAT yp = -sin(ang) * x + cos(ang) * y;

    // Now evaluate
    return 1 / sqrt(2*M_PI*sigmaX*sigmaY)*exp(-0.5*((xp / sigmaX)*(xp / sigmaX) +
                                                  (yp / sigmaY)*(yp / sigmaY)));
}

template class GaussianFunc<float>;
template class GaussianFunc<double>;

//template class gaussian2D<float>(float, float float, float, float, float, float);
//template class gaussian2D<double>(float, float float, float, float, float, float);