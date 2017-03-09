//
// Created by josem on 1/7/17.
//

#include <iostream>
#include "type.h"
#include "array.h"

using namespace em;

class ArrayImpl
{
public:
    ArrayDim adim;
    size_t msize = 0; // allocated memory size
    Type * typePtr = nullptr;
    void * valuePtr = nullptr;

    // Allocate enough memory to store all elements
    void allocate()
    {
        msize = adim.size() * typePtr->size();
        valuePtr = malloc(msize);
    }

    // Deallocate memory
    void deallocate()
    {
        if (msize > 0)
        {
            free(valuePtr);
            msize = 0;
        }
    }
};

ArrayDim::ArrayDim(size_t xdim, size_t ydim, size_t zdim, size_t ndim)
        :x(xdim), y(ydim), z(zdim), n(ndim) {}

size_t ArrayDim::size() const
{
    return x * y * z * n;
}

Array::Array(const ArrayDim &adim, Type *atype)
{
    implPtr->adim = adim;
    implPtr->typePtr = (atype != nullptr ? atype : Type::get<float>());
    implPtr->allocate();
}

Array::~Array()
{
    implPtr->deallocate();
}

void Array::resize(const ArrayDim &adim)
{
    implPtr->deallocate();
    implPtr->adim = adim;
    implPtr->allocate();
}

ArrayDim Array::getDimensions() const
{
    return implPtr->adim;
}