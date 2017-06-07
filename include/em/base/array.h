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
        // Default Ctor for empty dimensions
        ArrayDim();
        ArrayDim(size_t xdim, size_t ydim=1, size_t zdim=1, size_t ndim=1);
        size_t size() const;
        bool operator==(const ArrayDim &other);
    }; // class ArrayDim

    /** @ingroup base
     * Four dimensional array class to manage internal memory and data type.
     */
    class Array
    {
    public:
        /** Default constructor.
         * The Array will not have any data associated and
         * the type will be nullptr.
         */
        Array();

        // Constructor from dimensions and type
        Array(const ArrayDim &adim, ConstTypePtr type);

        /** Copy constructor from another Array.
         * This Array will have the same dimensions, data type
         * and values of the other Array.
         * @param other Other Array to be copied
         */
        Array(const Array &other);

        // Destructor
        virtual ~Array();

        // Assign operator
        Array& operator=(const Array &other);

        // String representation
        virtual std::string toString() const;

        // Dimensions
        virtual void resize(const ArrayDim &adim, ConstTypePtr type=nullptr);
        ArrayDim getDimensions() const;

        ConstTypePtr getType() const;

        template <class T>
        ArrayView<T> getView();

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
