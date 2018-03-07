//
// Created by josem on 1/7/17.
//

#include <iostream>
#include <sstream>
#include <cassert>

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
    return (ni > 0 && ni <= n && zi < z && yi < y && xi < x);
} // function ArrayDim.isValidIndex

bool ArrayDim::operator==(const ArrayDim &other) const
{
    return (x == other.x && y == other.y && z == other.z && n == other.n);
}

bool ArrayDim::operator!=(const ArrayDim &other) const
{
    return !(*this == other);
}

std::string ArrayDim::toString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
} // function ArrayDim.toString

int ArrayDim::getRank() const
{
    return (z > 1) ? 3 : (y > 1) ? 2 : 1;
} // function ArrayDim.getRank

std::ostream& em::operator<< (std::ostream &ostream, const ArrayDim &adim)
{
    ostream << "(" << adim.x << " x " << adim.y << " x "
            << adim.z << " x " << adim.n << ")";
    return ostream;
} // operator >> for ArrayDim

// ===================== Array::Impl Implementation =======================

class Array::Impl: public Type::Container
{
public:
    ArrayDim adim;
};// class ArrayImpl


// ===================== Array Implementation =======================

Array::Array()
{
    impl = new Impl();
} // empty Ctor

Array::Array(const ArrayDim &adim, const Type & type, void * memory): Array()
{
    // Type should be not null (another option could be assume float
    // or double by default)
    assert(!type.isNull());
    impl->adim = adim;
    allocate(type, adim.getSize(), memory);
} // Ctor for ArrayDim and const Type &

Array::Array(const Array &other): Array()
{
    *this = other;
} // Copy ctor Array

Array::~Array()
{
    delete impl;
} // Dtor Array

Array& Array::operator=(const Array &other)
{
    impl->adim = other.getDim();
    copyOrCast(other.getType(), impl->adim.getSize(), other);
    return *this;
} // function Array.operator=

bool Array::operator==(const Array &other) const
{
    auto& type = getType();
    auto dim = getDim();

    if (type != other.getType() || type.isNull() || dim != other.getDim())
        return false;

    return type.equals(getData(), other.getData(), dim.getSize());

} // function Array.operator==

bool Array::operator!=(const Array &other) const
{
    return !(*this == other);
} // function Array.operator!=

void Array::copy(const Array &other, const Type & type)
{
    auto finalType = !type.isNull() ? type :
                     getType().isNull() ? other.getType() : getType();
    impl->adim = other.getDim();
    copyOrCast(finalType, impl->adim.getSize(), other);
} // function Array.copy

Array Array::getAlias(size_t index)
{
    auto adim = getDim();
    ASSERT_ERROR((index < 0 || index > adim.n),
                 "Index should be betweeen zero and the number of elements.")

    void * data = getData();

    if (index > 0)
    {
        adim.n = 1; // Alias to a single item of this array
        data = (static_cast<uint8_t *>(data) +
                (index - 1) * adim.getItemSize() * getType().getSize());
    }
    //TODO: Consider aliasing to a single slice within a volume

    return Array(adim, getType(), data);
} // function Array.getAlias

ArrayDim Array::getDim() const
{
    return impl->adim;
} // function Array.getDim

void Array::resize(const ArrayDim &adim, const Type & type)
{
    //TODO: Avoid allocation if memory is enough !!!

    // Use type if not none, the current type if not
    impl->adim = adim;
    auto& finalType = type.isNull() ? getType() : type;
    allocate(finalType, adim.getSize());
} // function Array.resize

void Array::toStream(std::ostream &ostream) const
{
    auto data = static_cast<const char *>(getData());
    size_t n = impl->adim.getSize();
    size_t xdim = impl->adim.x;
    size_t ydim = impl->adim.y;
    auto type = getType();
    size_t typeSize = type.getSize();

    ostream << '[';

    for (size_t i = 0; i < ydim; ++i)
    {
        ostream << '[';
        type.toStream(data, ostream, xdim);

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

template <class T>
ArrayView<T> Array::getView()
{
    // Check the type is the same of the object
    assert(getType() == Type::get<T>());

    return ArrayView<T>(impl->adim, getData());
} // function Array.getView

std::ostream& em::operator<< (std::ostream &ostream, const Array &array)
{
    array.toStream(ostream);
    return ostream;
}

// ===================== ArrayView Implementation =======================
template <class T>
class ArrayView<T>::Impl
{
public:
    T * data;
    ArrayDim adim;
    size_t xy, xyz; // Cached values to speed-up indexing

    Impl(const ArrayDim &adim, void * rawMemory)
    {
        data = static_cast<T*>(rawMemory);
        this->adim = adim;
        xy = adim.getSliceSize();
        xyz = adim.getItemSize();
    }
}; // class ArrayView::Impl

#define GET_DATA() impl->data

template <class T>
ArrayView<T>::ArrayView(const ArrayDim &adim, void * rawMemory)
{
    impl = new Impl(adim, rawMemory);
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
T* ArrayView<T>::getData()
{
    return GET_DATA();
} // function ArrayView.getData

template <class T>
ArrayDim ArrayView<T>::getDim() const
{
    return impl->adim;
} // getDim


// ================ Explicit instantiations of Templates =======================
// This allows to implement template code in the .cpp

template em::ArrayView<int> em::Array::getView();
template em::ArrayView<float> em::Array::getView();
template em::ArrayView<double> em::Array::getView();

template class em::ArrayView<int>;
template class em::ArrayView<float>;
template class em::ArrayView<double>;