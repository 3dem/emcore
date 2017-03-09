//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_ARRAY_H
#define EM_CORE_ARRAY_H

#include <cstddef>


class ArrayImpl;

namespace em
{
    class Type;

    /**
     * Simple class to hold the Array dimensions.
     */
    class ArrayDim
    {
    public:
        size_t x, y, z, n;

    public:
        ArrayDim(size_t xdim, size_t ydim=1, size_t zdim=1, size_t ndim=1);
        size_t size() const;
    }; // class ArrayDim

    class Array
    {
    public:
        // Constructor and destructor
        Array(const ArrayDim &adim, Type *atype = nullptr);
        virtual ~Array();

        // Dimensions
        void resize(const ArrayDim &adim);
        ArrayDim getDimensions() const;

    private:
        ArrayImpl * implPtr;
    }; // class Array
}


#endif //EM_CORE_ARRAY_H
