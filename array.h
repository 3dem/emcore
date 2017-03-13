//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_ARRAY_H
#define EM_CORE_ARRAY_H

#include <cstddef>
#include <string>


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
        // Default Ctor for empty dimensions
        ArrayDim();
        ArrayDim(size_t xdim, size_t ydim=1, size_t zdim=1, size_t ndim=1);
        size_t size() const;
    }; // class ArrayDim


    class ArrayBase
    {
    public:
        virtual ~ArrayBase();

        //FIXME: Properly set this method as abstract;
        virtual std::string toString() const { return ""; };

        // Dimensions
        virtual void resize(const ArrayDim &adim);
        ArrayDim getDimensions() const;

        // Operators
        template <class T>
        ArrayBase& operator=(const T);
        // Return a raw pointer to data but doing some type checking
        template <class T>
        T * getDataPointer() const;

    protected:
        // Forbid to instantiate objects from ArrayBase class
        ArrayBase();
        // Pointer to implementation class, PIMPL idiom
        ArrayImpl * implPtr;
    }; // class ArrayBase


    template <class T>
    class Array: public ArrayBase
    {
    public:
        // Ctor and Dtor
        Array(const ArrayDim &adim);
        virtual ~Array();

        // Overridden functions from ArrayBase
        virtual std::string toString() const override;

        // Operators
        //template <class T2>
        void assign(const T &value);
        T& operator()(const int x, const int y=0, const int z=0, const size_t n=1);


    }; // class Array
}


#endif //EM_CORE_ARRAY_H
