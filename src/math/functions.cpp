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

template class em::GaussianFunc<float>;
template class em::GaussianFunc<double>;
