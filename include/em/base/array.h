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

        // Copy constructor from another ArrayDim
        ArrayDim(const ArrayDim &adim);

        // Constructor from a given dimensions
        explicit ArrayDim(size_t xdim, size_t ydim=1, size_t zdim=1, size_t ndim=1);

        bool operator==(const ArrayDim &other);

        /** Return the total number of pixels/voxels of an 4D-array with these
         * dimensions. (i.e. x * y * z * n).
         * To obtain the memory size, the returned value should be multiplied
         * by the size of the datatype of the Array.
         */
        size_t getSize() const;

        /** Return the number of pixels/voxels of a single item (either 2D or 3D)
         * in the Array. (i.e. x * y * z)
         */
        size_t getItemSize() const;
    }; // class ArrayDim

    std::ostream& operator<< (std::ostream &ostream, const ArrayDim &adim);

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

        // Dimensions
        virtual void resize(const ArrayDim &adim, ConstTypePtr type=nullptr);
        ArrayDim getDimensions() const;

        /** Return a constant pointer to underlying Type object. */
        ConstTypePtr getType() const;

        /** Return a pointer to the internal data.
         * Use this function with care. It is intended to be used by
         * ImageReader subclasses to allow read from disk and put
         * directly into memory without need of a buffer.
         *
         * For other memory manipulation, use the provided
         * functions to copy element into the array from
         * a given memory location.
         */
        void * getDataPointer();

        template <class T>
        ArrayView<T> getView();

        // String representation
        virtual void toStream(std::ostream &ostream) const;
        virtual std::string toString() const;

    protected:
        // Pointer to implementation class, PIMPL idiom
        ArrayImpl * implPtr;
    }; // class Array

    std::ostream& operator<< (std::ostream &ostream, const Array &array);

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
