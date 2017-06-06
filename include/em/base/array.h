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
    template <class T> class ArrayView;

    /** @ingroup base
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

    /** @ingroup base
     * Four dimensional array class to manage internal memory and data type.
     */
    class Array
    {
    public:
        // Ctor and Dtor
        Array(const ArrayDim &adim, ConstTypePtr  type= nullptr);
        virtual ~Array();

        virtual std::string toString() const;

        // Dimensions
        virtual void resize(const ArrayDim &adim, ConstTypePtr type=nullptr);
        ArrayDim getDimensions() const;

//        // Operators
//        template <class T>
//        Array& operator=(const T);
//
//        template <class T>
//        T& operator()(const int x, const int y=0, const int z=0, const size_t n=1);
//
        template <class T>
        ArrayView<T> getView();

        // Assign a constant value to all the array
        template <class T>
        void assign(const T &value);

    protected:
        // Pointer to implementation class, PIMPL idiom
        ArrayImpl * implPtr;

    }; // class Array

    /** @ingroup base
     *  View of an Array that is parametrized.
     */
    template <class T>
    class ArrayView
    {
    public:
        ArrayView() = default;
        ArrayView(const ArrayView &aview) = default;

        std::string toString() const;
        T& operator()(const int x, const int y=0, const int z=0, const size_t n=1);
        void assign(const T &value);
        T * getDataPointer();

    private:
        // Only friend class Array can create ArrayView objects
        ArrayView(const ArrayDim &adim, void * rawMemory);

        T * data = nullptr;
        ArrayDim adim;

    friend class Array;
    }; // class ArrayView<T>

} // namespace em


#endif //EM_CORE_ARRAY_H
