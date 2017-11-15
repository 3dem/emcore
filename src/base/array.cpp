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

ArrayDim::ArrayDim() :x(0), y(1), z(1), n(1) {}

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

size_t ArrayDim::getSliceSize() const
{
    return x * y;
} // function ArrayDim.getSliceSize

bool ArrayDim::isValidIndex(size_t xi, size_t yi, size_t zi, size_t ni)
{
    // We don't compare xi, yi and zi >= 0 because unsigned int values
    // are always greater than 0
    std::cerr << "isValidIndex " << *this << std::endl;

    return (ni > 0 && ni <= n && zi < z && yi < y && xi < x);
} // function ArrayDim.isValidIndex

bool ArrayDim::operator==(const ArrayDim &other) const
{
    return (x == other.x and y == other.y and z == other.z and n == other.n);
}

bool ArrayDim::operator!=(const ArrayDim &other) const
{
    return !(*this == other);
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
    // Flag to know when the memory is "owned" by this array and when it should
    // take care of freeing it.
    bool ownsMemory = true;

    // If the memory pointer is nullptr, allocate enough memory to store all
    // elements. If not, point data to there and mark as memory not owned
    // by this array
    void allocate(const ArrayDim &adim, ConstTypePtr type,
                  void * memory= nullptr)
    {
        deallocate(); // Release memory if necessary
        this->adim = adim;
        this->typePtr = type;
        msize = adim.getSize() * typePtr->getSize();
        // Asign ownsMemory flag and dataPtr in the same statement
        dataPtr = (ownsMemory = (memory == nullptr)) ? malloc(msize) : memory;
    }

    // Deallocate memory
    void deallocate()
    {
        if (ownsMemory && msize > 0)
        {
            free(dataPtr);
            msize = 0;
        }
    }

    void copy(const ArrayDim &adim, ConstTypePtr type,
              const ArrayImpl *other)
    {
        allocate(adim, type);

        if (type == other->typePtr)
            type->copy(other->dataPtr, dataPtr, adim.getSize());
        else
            type->cast(other->dataPtr, dataPtr, adim.getSize(), other->typePtr);
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

Array::Array(const ArrayDim &adim, ConstTypePtr type, void * memory)
{
    implPtr = new ArrayImpl();
    // Type should be not null (another option could be assume float
    // or double by default)
    assert(type != nullptr);
    implPtr->allocate(adim, type, memory);
} // Ctor for ArrayDim and ConstTypePtr

Array::Array(const Array &other)
{
    implPtr = new ArrayImpl();
    *this = other;
} // Copy ctor Array

Array::~Array()
{
    delete implPtr;
} // Dtor Array

Array& Array::operator=(const Array &other)
{
    implPtr->copy(other.getDim(), other.getType(), other.implPtr);
    return *this;
} // function Array.operator=

bool Array::operator==(const Array &other) const
{
    auto type = getType();
    auto dim = getDim();

    if (type != other.getType() || type == nullptr || dim != other.getDim())
        return false;

    return type->equals(getDataPointer(), other.getDataPointer(), dim.getSize());

} // function Array.operator==

bool Array::operator!=(const Array &other) const
{
    return !(*this == other);
} // function Array.operator!=

void Array::copy(const Array &other, ConstTypePtr type)
{
    auto finalType = type != nullptr ? type :
                     implPtr->typePtr == nullptr ? other.getType() :
                     implPtr->typePtr;
    implPtr->copy(other.getDim(), finalType, other.implPtr);
} // function Array.copy

Array Array::getAlias(size_t index)
{
    auto adim = getDim();
    ASSERT_ERROR((index < 0 or index > adim.n),
                 "Index should be betweeen zero and the number of elements.")

    void * data = getDataPointer();

    if (index > 0)
    {
        adim.n = 1; // Alias to a single item of this array
        data = (static_cast<uint8_t *>(data) +
                (index - 1) * adim.getItemSize() * getType()->getSize());
    }
    //TODO: Consider aliasing to a single slice within a volume

    return Array(adim, getType(), data);
} // function Array.getAlias

ArrayDim Array::getDim() const
{
    return implPtr->adim;
} // function Array.getDim

void Array::resize(const ArrayDim &adim, ConstTypePtr type)
{
    // Use type if not none, the current type if not
    implPtr->allocate(adim, type == nullptr ? implPtr->typePtr : type);
} // function Array.resize

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
        ostream << '[';
        implPtr->typePtr->toStream(data, ostream, xdim);
        if (i < ydim-1)
            ostream << ']' << std::endl;
        else
            ostream << "]]" << std::endl;
        data += xdim * typeSize; // Increment point row by row
    }

    //ostream << ']';
} // function Array.toStream

std::string Array::toString() const
{
    std::stringstream ss;
    toStream(ss);
    return ss.str();
} // function Array.toString

ConstTypePtr Array::getType() const
{
    return implPtr->typePtr;
} // function Array.getType

void * Array::getDataPointer()
{
    return implPtr->dataPtr;
} // function Array.getDataPointer

const void * Array::getDataPointer() const
{
    return implPtr->dataPtr;
} // function Array.getDataPointer

template <class T>
ArrayView<T> Array::getView()
{
    ConstTypePtr valueTypePtr = Type::get<T>();
    // Check the type is the same of the object
    assert(implPtr->typePtr == valueTypePtr);

    return ArrayView<T>(implPtr->adim, implPtr->dataPtr);
} // function Array.getView

std::ostream& em::operator<< (std::ostream &ostream, const Array &array)
{
    array.toStream(ostream);
    return ostream;
}

// ===================== ArrayView Implementation =======================
class ArrayViewImpl
{
public:
    void * data;
    ArrayDim adim;
    size_t xy, xyz; // Cached values to speed-up indexing

    ArrayViewImpl(const ArrayDim &adim, void * rawMemory)
    {
        data = rawMemory;
        this->adim = adim;
        xy = adim.getSliceSize();
        xyz = adim.getItemSize();
    }
}; // class ArrayViewImpl

#define GET_DATA() static_cast<T*>(impl->data)

template <class T>
ArrayView<T>::ArrayView(const ArrayDim &adim, void * rawMemory)
{
    impl = new ArrayViewImpl(adim, rawMemory);
} // Ctor ArrayView

template <class T>
void ArrayView<T>::assign(const T &value)
{
    T *ptr = GET_DATA();
    size_t n = impl->adim.getSize();

    for (size_t i = 0; i < n; ++i, ++ptr)
        *ptr = value;
} // function ArrayView.assign

template <class T>
ArrayView<T>::~ArrayView()
{
    delete impl;
}

template <class T>
T& ArrayView<T>::operator()(int x, int y, int z, size_t n)
{
    ASSERT_ERROR(!getDim().isValidIndex(x, y, z, n),
                 "Invalid indexes for this array. ");

    T *ptr = GET_DATA();
    ptr += (n - 1) * impl->xyz + z * impl->xy + y * x + x;
    return *ptr;
} // function ArrayView.operator()

template <class T>
std::string ArrayView<T>::toString() const
{
    std::stringstream ss;

    T *ptr = GET_DATA();
    size_t n = impl->adim.getSize();
    size_t xdim = impl->adim.x;

    ss << "[";

    for (size_t i = 0; i < n; ++i, ++ptr)
    {
        ss << *ptr << " ";
        if (i % xdim == xdim-1)
            ss << "\n";
    }

    ss << "]";

    return ss.str();
} // function ArrayView.toString

template <class T>
T* ArrayView<T>::getDataPointer()
{
    return GET_DATA();
} // function ArrayView.getDataPointer

template <class T>
ArrayDim ArrayView<T>::getDim() const
{
    return impl->adim;
} // getDim


// ================ Explicit instantiations of Templates ==============================
// This allows to implement template code in the .cpp

template em::ArrayView<int> em::Array::getView();
template em::ArrayView<float> em::Array::getView();
template em::ArrayView<double> em::Array::getView();

template class em::ArrayView<int>;
template class em::ArrayView<float>;
template class em::ArrayView<double>;