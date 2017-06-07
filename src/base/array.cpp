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

bool ArrayDim::operator==(const ArrayDim &other)
{
    return (x == other.x and y == other.y and z == other.z and n == other.n);
}


// ===================== ArrayImpl Implementation =======================

class ArrayImpl
{
public:
    ArrayDim adim;
    size_t msize = 0; // allocated memory size
    ConstTypePtr typePtr = nullptr;
    void * dataPtr = nullptr;

    // Allocate enough memory to store all elements
    void allocate(const ArrayDim &adim, ConstTypePtr type)
    {
        this->adim = adim;
        this->typePtr = type;
        msize = adim.size() * typePtr->size();
        dataPtr = malloc(msize);
        std::cout << "this: " << this << " allocate: msize: " << msize << " dataPtr: " << (dataPtr == nullptr) << std::endl;


    }

    // Deallocate memory
    void deallocate()
    {
        std::cout << "this: " << this << " deallocate: msize: " << msize << " dataPtr: " << (dataPtr == nullptr) << std::endl;
        if (msize > 0)
        {
            free(dataPtr);
            msize = 0;
        }
    }

    ~ArrayImpl()
    {
        deallocate();
    }
};// class ArrayImpl


// ===================== Array Implementation =======================

Array::Array()
{
    implPtr = new ArrayImpl();
} // empty Ctor

Array::Array(const ArrayDim &adim, ConstTypePtr type)
{
    implPtr = new ArrayImpl();
    // Type should be not null (another option could be assume float or double by default)
    assert(type != nullptr);
    implPtr->allocate(adim, type);
} // Ctor for ArrayDim and ConstTypePtr

Array::Array(const Array &other)
{
    implPtr = new ArrayImpl();
    *this = other;
} // Copy ctor Array

Array::~Array()
{
    std::cout << "this: " << this << " ~Array()" << std::endl;
    delete implPtr;
} // Dtor


Array& Array::operator=(const Array &other)
{
    std::cout << "Assigning array..." << std::endl;
    ConstTypePtr typePtr = other.getType();
    implPtr->allocate(other.getDimensions(), typePtr);
    typePtr->copy(other.implPtr->dataPtr, implPtr->dataPtr, implPtr->adim.size());
    return *this;
} //operator=

ArrayDim Array::getDimensions() const
{
    return implPtr->adim;
} // getDimensions

void Array::resize(const ArrayDim &adim, ConstTypePtr type)
{
    implPtr->deallocate();
    // Use type if not none, the current type if not
    implPtr->allocate(adim, type == nullptr ? implPtr->typePtr : type);
}

std::string Array::toString() const
{
    std::stringstream ss;

//    T *ptr = getDataPointer<T>();
//    T *ptrIter = ptr;
//    size_t n = implPtr->adim.size();
//    size_t xdim = implPtr->adim.x;

    ss << "[";

//    for (size_t i = 0; i < n; ++i, ++ptrIter)
//    {
//        ss << *ptrIter << " ";
//        if (i % xdim == xdim-1)
//            ss << "\n";
//    }

    ss << "]";

    return ss.str();
}

ConstTypePtr Array::getType() const
{
    return implPtr->typePtr;
}

template <class T>
ArrayView<T> Array::getView()
{
    ConstTypePtr valueTypePtr = Type::get<T>();
    // Check the type is the same of the object
    assert(implPtr->typePtr == valueTypePtr);

    return ArrayView<T>(implPtr->adim, implPtr->dataPtr);
}

// ===================== ArrayView Implementation =======================

template <class T>
ArrayView<T>::ArrayView(const ArrayDim &adim, void * rawMemory)
{
    this->data = static_cast<T*>(rawMemory);
    this->adim = adim;
}

template <class T>
void ArrayView<T>::assign(const T &value)
{
    T *ptrIter = data;
    size_t n = adim.size();

    for (size_t i = 0; i < n; ++i, ++ptrIter)
        *ptrIter = value;
}

template <class T>
T& ArrayView<T>::operator()(int x, int y, int z, size_t n)
{
    T *ptr = data;
    ptr += x + y * adim.y;
    return *ptr;
}

template <class T>
std::string ArrayView<T>::toString() const
{
    std::stringstream ss;

    T *ptrIter = data;
    size_t n = adim.size();
    size_t xdim = adim.x;

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
T* ArrayView<T>::getDataPointer()
{
    return data;
}

// ================ Explicit instantiations of Templates ==============================
// This allows to implement template code in the .cpp

template em::ArrayView<int> em::Array::getView();
template em::ArrayView<float> em::Array::getView();
template em::ArrayView<double> em::Array::getView();

template class em::ArrayView<int>;
template class em::ArrayView<float>;
template class em::ArrayView<double>;