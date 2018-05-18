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

class Array::Impl
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
    copyOrCast(other, impl->adim.getSize());
    return *this;
} // function Array.operator= Array

Array& Array::operator=(const Object &value)
{
    copyOrCast(value, impl->adim.getSize(), true);
    return *this;
} // function Array.operator= Object

Array& Array::operator+=(const Array &other)
{
    auto& dim = impl->adim;
    ASSERT_ERROR(dim != other.getDim(),
                 "Arrays should have the same dimensions.")
    getType().operate(Type::ADD, other.getData(), other.getType(), getData(),
                      dim.getSize());
    return *this;
} // function Array.operator+= Array

Array& Array::operator+=(const Object &value)
{
    getType().operate(Type::ADD, value.getData(), value.getType(), getData(),
                      impl->adim.getSize(), true);
    return *this;
} // function Array.operator+= Object

Array& Array::operator-=(const Array &other)
{
    auto& dim = impl->adim;
    ASSERT_ERROR(dim != other.getDim(),
                 "Arrays should have the same dimensions.")
    getType().operate(Type::SUB, other.getData(), other.getType(), getData(),
                      dim.getSize());
    return *this;
} // function Array.operator-= Array

Array& Array::operator-=(const Object &value)
{
    getType().operate(Type::SUB, value.getData(), value.getType(), getData(),
                      impl->adim.getSize(), true);
    return *this;
} // function Array.operator-= Object

Array& Array::operator*=(const Array &other)
{
    auto& dim = impl->adim;
    ASSERT_ERROR(dim != other.getDim(),
                 "Arrays should have the same dimensions.")
    getType().operate(Type::MUL, other.getData(), other.getType(), getData(),
                      dim.getSize());
    return *this;
} // function Array.operator*= Array

Array& Array::operator*=(const Object &value)
{
    getType().operate(Type::MUL, value.getData(), value.getType(), getData(),
                      impl->adim.getSize(), true);
    return *this;
} // function Array.operator*= Object

Array& Array::operator/=(const Array &other)
{
    auto& dim = impl->adim;
    ASSERT_ERROR(dim != other.getDim(),
                 "Arrays should have the same dimensions.")
    getType().operate(Type::DIV, other.getData(), other.getType(), getData(),
                      dim.getSize());
    return *this;
} // function Array.operator/= Array

Array& Array::operator/=(const Object &value)
{
    getType().operate(Type::DIV, value.getData(), value.getType(), getData(),
                      impl->adim.getSize(), true);
    return *this;
} // function Array.operator/= Object

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



Array Array::getView(size_t index)
{
    auto adim = getDim();
    ASSERT_ERROR((index < 0 || index > adim.n),
                 "Index should be betweeen zero and the number of elements.")

    void * data = getData();

    if (index > 0)
    {
        adim.n = 1; // View of a single item of this array
        data = (static_cast<uint8_t *>(data) +
                (index - 1) * adim.getItemSize() * getType().getSize());
    }
    //TODO: Consider aliasing to a single slice within a volume

    return Array(adim, getType(), data);
} // function Array.getView

ArrayDim Array::getDim() const
{
    return impl->adim;
} // function Array.getDim

void Array::resize(const ArrayDim &adim, const Type & type)
{
    // Use input type if not none, otherwise the current type of the array
    impl->adim = adim;
    auto& finalType = type.isNull() ? getType() : type;
    // Validate that either the input type or the current type are not null
    ASSERT_ERROR(finalType.isNull(),
                 "Input type should not be null if the Array does not have "
                 "a type yet.")
    allocate(finalType, adim.getSize());
} // function Array.resize

void Array::resize(const Array &other)
{
    resize(other.getDim(), other.getType());
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
ArrayT<T> Array::getView()
{
    // Check the type is the same of the object
    assert(getType() == Type::get<T>());

    return ArrayT<T>(impl->adim, getData());
} // function Array.getView

std::ostream& em::operator<< (std::ostream &ostream, const Array &array)
{
    array.toStream(ostream);
    return ostream;
}

// ===================== ArrayT Implementation =======================
template <class T>
class ArrayT<T>::Impl
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
}; // class ArrayT::Impl

#define GET_DATA() impl->data

template <class T>
ArrayT<T>::ArrayT(const ArrayDim &adim, void * rawMemory)
{
    impl = new Impl(adim, rawMemory);
} // Ctor ArrayT

template <class T>
void ArrayT<T>::assign(const T &value)
{
    T *ptr = GET_DATA();
    size_t n = impl->adim.getSize();

    for (size_t i = 0; i < n; ++i, ++ptr)
        *ptr = value;
} // function ArrayT.assign

template <class T>
ArrayT<T>::~ArrayT()
{
    delete impl;
}

template <class T>
T& ArrayT<T>::operator()(int x, int y, int z, size_t n)
{
    ASSERT_ERROR(!getDim().isValidIndex(x, y, z, n),
                 "Invalid indexes for this array. ");

    T *ptr = GET_DATA();
    ptr += (n - 1) * impl->xyz + z * impl->xy + y * impl->adim.x + x;
    return *ptr;
} // function ArrayT.operator()

template <class T>
std::string ArrayT<T>::toString() const
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
} // function ArrayT.toString

template <class T>
T* ArrayT<T>::getData()
{
    return GET_DATA();
} // function ArrayT.getData

template <class T>
ArrayDim ArrayT<T>::getDim() const
{
    return impl->adim;
} // getDim


// ================ Explicit instantiations of Templates =======================
// This allows to implement template code in the .cpp

template em::ArrayT<int8_t > em::Array::getView();
template em::ArrayT<uint8_t > em::Array::getView();
template em::ArrayT<int16_t > em::Array::getView();
template em::ArrayT<uint16_t > em::Array::getView();
template em::ArrayT<int32_t > em::Array::getView();
template em::ArrayT<uint32_t > em::Array::getView();

template em::ArrayT<float> em::Array::getView();
template em::ArrayT<double> em::Array::getView();
template em::ArrayT<cfloat> em::Array::getView();
template em::ArrayT<cdouble> em::Array::getView();

template class em::ArrayT<int8_t>;
template class em::ArrayT<uint8_t>;
template class em::ArrayT<int16_t>;
template class em::ArrayT<uint16_t>;
template class em::ArrayT<int32_t>;
template class em::ArrayT<uint32_t>;

template class em::ArrayT<float>;
template class em::ArrayT<double>;
template class em::ArrayT<cfloat>;
template class em::ArrayT<cdouble>;