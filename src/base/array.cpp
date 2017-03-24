//
// Created by josem on 1/7/17.
//

#include <iostream>
#include <sstream>
#include <cassert>

#include "em/base/type.h"
#include "em/base/array.h"

using namespace em;


// ===================== ArrayDim Implementation =======================

ArrayDim::ArrayDim() :x(0), y(0), z(0), n(0) {}

ArrayDim::ArrayDim(size_t xdim, size_t ydim, size_t zdim, size_t ndim)
        :x(xdim), y(ydim), z(zdim), n(ndim) {}

size_t ArrayDim::size() const
{
    return x * y * z * n;
}


// ===================== ArrayImpl Implementation =======================

class ArrayImpl
{
public:
    ArrayDim adim;
    size_t msize = 0; // allocated memory size
    Type * typePtr = nullptr;
    void * dataPtr = nullptr;

    // Allocate enough memory to store all elements
    void allocate()
    {
        msize = adim.size() * typePtr->size();
        dataPtr = malloc(msize);
    }

    // Deallocate memory
    void deallocate()
    {
        if (msize > 0)
        {
            free(dataPtr);
            msize = 0;
        }
    }
};


// ===================== ArrayBase Implementation =======================

ArrayBase::ArrayBase()
{
    implPtr = new ArrayImpl();
}

ArrayBase::~ArrayBase()
{
    implPtr->deallocate();
    delete implPtr;
}

ArrayDim ArrayBase::getDimensions() const
{
    return implPtr->adim;
}

void ArrayBase::resize(const ArrayDim &adim)
{
    implPtr->deallocate();
    implPtr->adim = adim;
    implPtr->allocate();
}

template <class T>
T* ArrayBase::getDataPointer() const
{
    Type *valueTypePtr = Type::get<T>();

    // Check the type is the same of the object
    assert(implPtr->typePtr == valueTypePtr);

    // Cast the void* to the specific pointer type
    return static_cast<T*>(implPtr->dataPtr);
}


// ===================== Array<T> Implementation =======================

template <class T>
Array<T>::Array(const ArrayDim &adim)
{
    implPtr->adim = adim;
    implPtr->typePtr = Type::get<T>();
    implPtr->allocate();
}

template <class T>
Array<T>::~Array()
{
    // implPtr->deallocate();
}

template <class T>
std::string Array<T>::toString() const
{
    std::stringstream ss;

    T *ptr = getDataPointer<T>();
    T *ptrIter = ptr;
    size_t n = implPtr->adim.size();
    size_t xdim = implPtr->adim.x;

    ss << "[";

    for (size_t i = 0; i < n; ++i, ++ptrIter)
    {
        ss << *ptrIter << " ";
        if (i % xdim == xdim-1)
            ss << "\n";
    }

    ss << "]";

    return ss.str();

}

template <class T>
void Array<T>::assign(const T &value)
{
    T *ptr = getDataPointer<T>();
    T *ptrIter = ptr;
    size_t n = implPtr->adim.size();

    for (size_t i = 0; i < n; ++i, ++ptrIter)
    {
        *ptrIter = value;
    }
}

template <class T>
T& Array<T>::operator()(int x, int y, int z, size_t n)
{
    T *ptr = getDataPointer<T>();
    ArrayDim &adim = implPtr->adim;

    ptr += x + y * adim.y;
    return *ptr;
}


// Explicit instantiations of Array class in order to allows
// its implementation in the .cpp file
template class em::Array<float>;
template class em::Array<double>;
template class em::Array<int>;