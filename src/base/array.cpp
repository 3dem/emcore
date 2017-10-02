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

ArrayDim::ArrayDim(const ArrayDim &adim)
        :x(adim.x), y(adim.y), z(adim.z), n(adim.n) {}

ArrayDim::ArrayDim(size_t xdim, size_t ydim, size_t zdim, size_t ndim)
        :x(xdim), y(ydim), z(zdim), n(ndim) {}

size_t ArrayDim::getSize() const
{
    return x * y * z * n;
}

size_t ArrayDim::getItemSize() const
{
    return x * y * z;
}

bool ArrayDim::operator==(const ArrayDim &other)
{
    return (x == other.x and y == other.y and z == other.z and n == other.n);
}

std::ostream& em::operator<< (std::ostream &ostream, const ArrayDim &adim)
{
    ostream << "(" << adim.x << " x " << adim.y << " x "
            << adim.z << " x " << adim.n << ")";
    return ostream;
}

// ===================== ArrayImpl Implementation =======================

class ArrayImpl
{
public:
    ArrayDim adim;
    size_t msize = 0; // allocated memory getSize
    ConstTypePtr typePtr = nullptr;
    void * dataPtr = nullptr;

    // Allocate enough memory to store all elements
    void allocate(const ArrayDim &adim, ConstTypePtr type)
    {
        this->adim = adim;
        this->typePtr = type;
        msize = adim.getSize() * typePtr->getSize();
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
    // Type should be not null (another option could be assume float
    // or double by default)
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
    std::cout << "Assigning Array..." << std::endl;
    ConstTypePtr typePtr = other.getType();
    resize(other.getDimensions(), typePtr);
    typePtr->copy(other.implPtr->dataPtr, implPtr->dataPtr,
                  implPtr->adim.getSize());
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

void Array::toStream(std::ostream &ostream) const
{
    char * data = static_cast<char*>(implPtr->dataPtr);
    size_t n = implPtr->adim.getSize();
    size_t xdim = implPtr->adim.x;
    size_t ydim = implPtr->adim.y;
    size_t typeSize = implPtr->typePtr->getSize();

    ostream << '[';

    for (size_t i = 0; i < ydim; ++i)
    {
        implPtr->typePtr->toStream(data, ostream, xdim);
        ostream << std::endl;
        data += xdim * typeSize; // Increment point row by row
    }

    ostream << ']';
}

std::string Array::toString() const
{
    std::stringstream ss;
    toStream(ss);
    return ss.str();
}

ConstTypePtr Array::getType() const
{
    return implPtr->typePtr;
} // getType

void * Array::getDataPointer()
{
    return implPtr->dataPtr;
} // getDataPointer

template <class T>
ArrayView<T> Array::getView()
{
    ConstTypePtr valueTypePtr = Type::get<T>();
    // Check the type is the same of the object
    assert(implPtr->typePtr == valueTypePtr);

    return ArrayView<T>(implPtr->adim, implPtr->dataPtr);
}

std::ostream& em::operator<< (std::ostream &ostream, const Array &array)
{
    array.toStream(ostream);
    return ostream;
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
    size_t n = adim.getSize();

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
    size_t n = adim.getSize();
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