//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_ARRAY_H
#define EM_CORE_ARRAY_H

#include <cstddef>
#include <string>

#include "em/base/object.h"
#include "em/base/container_priv.h"


namespace em
{
    class Type;
    template <class T> class ArrayT;

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

        /** Return the rank of this dimension.
         * 1 if y = z = 1
         * 2 if z = 1
         * 3 otherwise
         */
         int getRank() const;

    }; // class ArrayDim

    std::ostream& operator<< (std::ostream &ostream, const ArrayDim &adim);

    /** @ingroup base
     * Four dimensional Array class to manage internal memory and data type.
     */
    class Array: public TypedContainer
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
         * When providing the memory location, the created array could be seen
         * as an "alias" of another array of a portion of it. In this case, the
         * memory is considered not owned by this Array and it will not be freed.
         */
        Array(const ArrayDim &adim, const Type & type, void * memory = nullptr);

        /** Copy constructor from another Array.
         * This Array will have the same dimensions, data type
         * and values of the other Array.
         * @param other Other Array to be copied
         */
        Array(const Array &other);

        /** Move constructor */
        Array(Array &&other);

        // Destructor
        virtual ~Array();

        /** Assign other Array to this one. This operation will resize the
         * current Array to have the same type and dimensions of the other Array.
         * After the asignment both Array should be equal.
         *
         * @param other Other Array from which the elements will be assigned
         * @return *this
         */
        Array& operator=(const Array& other);

        /** Move assignment */
        Array& operator=(Array&& other);

        /** Copy all the elements from the given array.
         * The calling array will be resized to have the same dimensions
         * of the input array. All elements from the input array will be copied or
         * casted to the output.
         * @param other Other Array from which the elements will be copied
         * @param type If is typeNull, then the current type of the output array
         * will be used. If it is also null, then the input array type will be
         * used.
         */
         void copy(const Array& other, const Type& type=typeNull);

        /** Assign the value of a single element to the values of the array.
         * If the Array type is the same of the input Object type, then the
         * elements will be copied. If not, they will be casted.
         * @param value Input Object with the value that will be assigned
         * @return *this
         */
        void set(const Object &value);

        // Implement some arithmetic operators using as input another
        // Array and single Object value.
        Array& operator+=(const Array& other);
        Array& operator+=(const Object& value);
        Array& operator-=(const Array& other);
        Array& operator-=(const Object& value);
        Array& operator*=(const Array& other);
        Array& operator*=(const Object& value);
        Array& operator/=(const Array& other);
        Array& operator/=(const Object& value);

        bool operator==(const Array &other) const;
        bool operator!=(const Array &other) const;

        /** Return a View array that share the memory with this one.
         * If index is 0, the new Array will have exactly the same dimensions.
         * If index is between 1 and n, then it will point to a single item
         * but still with the same x, y and z dimensions.
         * @param index If 0 the whole Array will be aliased, if not, just
         * a single item.
         * @return Array aliased that share the same memory
         */
        Array getView(size_t index = 0);

        /** Change the dimensions of the current Array.
         * This operation usually imply a new allocation of memory.
         * Optionally, a new type can be passed.
         * If the current array does not have a type yet, then the input type
         * should be different from typeNull.
         */
        void resize(const ArrayDim& adim, const Type& type=typeNull);

        /** Resize the array based on the dimensions and the type of the
         * input Array.
         */
         void resize(const Array& other);

        /** Return the current dimensions of the Array */
        ArrayDim getDim() const;

        template <class T>
        ArrayT<T> getView();

        // String representation
        virtual void toStream(std::ostream &ostream) const;
        virtual std::string toString() const;

    protected:
        // Pointer to implementation class, PIMPL idiom
        class Impl;
        Impl * impl;

    }; // class Array

    std::ostream& operator<< (std::ostream &ostream, const Array &array);

    /** @ingroup base
     *  View of an Array that is parametrized.
     */
    template <class T>
    class ArrayT
    {
    public:
        ArrayT() = default;
        ArrayT(const ArrayT &aview) = default;
        ~ArrayT();

        std::string toString() const;
        T& operator()(const int x, const int y=0, const int z=0,
                      const size_t n=1);
        void assign(const T &value);
        T * getData();
        const T* getData() const;
        ArrayDim getDim() const;

    private:
        // Only friend class Array can create ArrayT objects
        ArrayT(const ArrayDim &adim, void * rawMemory);
        class Impl;
        Impl * impl;

    friend class Array;
    }; // class ArrayT<T>

} // namespace em


#endif //EM_CORE_ARRAY_H
