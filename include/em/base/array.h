//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_ARRAY_H
#define EM_CORE_ARRAY_H

#include <cstddef>
#include <string>

#include "em/base/type.h"


class ArrayImpl;
class ArrayViewImpl;


namespace em
{
    class Type;
    template <class T> class ArrayView;

    /** @ingroup base
     * Simple class to hold an Array dimensions.
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
        explicit ArrayDim(size_t xdim, size_t ydim=1, size_t zdim=1,
                          size_t ndim=1);

        // Comparison operators
        bool operator==(const ArrayDim &other) const;
        bool operator!=(const ArrayDim &other) const;

        /** Return the total number of pixels/voxels of an 4D-array with these
         * dimensions. (i.e. x * y * z * n).
         * To obtain the memory size, the returned value should be multiplied
         * by the size of the datatype of the Array.
         */
        size_t getSize() const;

        /** Return the number of pixels/voxels of a single item
         * (either 2D or 3D) in the Array. (i.e. x * y * z)
         */
        size_t getItemSize() const;

        /** Return the number of pixels is a slice of the volume.
         * (i.e. x * y value)
         */
         size_t getSliceSize() const;

        /** Return if some indexes are within the boundaries of these
         * dimensions.
         */
         bool isValidIndex(size_t xi, size_t yi=0, size_t zi=0,
                           size_t ni=1);

        /** Simple string representation */
        std::string toString() const;

    }; // class ArrayDim

    std::ostream& operator<< (std::ostream &ostream, const ArrayDim &adim);

    /** @ingroup base
     * Four dimensional Array class to manage internal memory and data type.
     */
    class Array
    {
    public:
        /** Default constructor.
         *
         * The Array will not have any data associated and the type will be
         * nullptr.
         */
        Array();

        /** Create an Array by providing dimensions and type.
         * Optionally, the memory address could be provided.
         *
         * This constructor should be used with care when providing the array
         * memory address. It should be guaranteed that the memory address
         * points to a place allocated for the given number of elements and
         * size specified by ArrayDim and Type.
         *
         * When using this function, the created array could be seen as an
         * "alias" of another array of a portion of it. In this case, the
         * memory is considered not owned by this Array and it will not be freed.
         */
        Array(const ArrayDim &adim, ConstTypePtr type, void * memory = nullptr);

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

        // Comparison operator
        bool operator==(const Array &other) const;
        bool operator!=(const Array &other) const;

        /**
         * Copy the elements from another array. The input type, if not null,
         * will be used for the resulting elements. If type is null, the type
         * of this array will be used. If the type of the array is also null,
         * then the type of the input array will be used.
         * @param other The other array from which to copy the elements.
         * @param type Type to be used for resulting elements, if null, use the
         * current type of the array.
         */
        void copy(const Array &other, ConstTypePtr type=nullptr);

        /** Return an "aliased" Array that share the memory with this one.
         * If index is 0, the new Array will have exactly the same dimensions.
         * If index is between 1 and n, then it will point to a single item
         * but still with the same x, y and z dimensions.
         * @param index If 0 the whole Array will be aliased, if not, just
         * a single item.
         * @return Array aliased that share the same memory
         */
        Array getAlias(size_t index = 0);

        // Dimensions
        virtual void resize(const ArrayDim &adim, ConstTypePtr type=nullptr);
        ArrayDim getDim() const;

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

        const void * getDataPointer() const;

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
        ~ArrayView();

        std::string toString() const;
        T& operator()(const int x, const int y=0, const int z=0,
                      const size_t n=1);
        void assign(const T &value);
        T * getDataPointer();
        ArrayDim getDim() const;

    private:
        // Only friend class Array can create ArrayView objects
        ArrayView(const ArrayDim &adim, void * rawMemory);

        ArrayViewImpl * impl;

    friend class Array;
    }; // class ArrayView<T>

} // namespace em


#endif //EM_CORE_ARRAY_H
