//
// Created by josem on 1/7/17.
//

#include <iostream>
#include <sstream>
#include <cassert>

#include "emc/base/array.h"

using namespace emcore;

namespace emc = emcore;


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

std::ostream& emc::operator<< (std::ostream &ostream, const ArrayDim &adim)
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

Array::Array(Array &&other)
{
    swap(std::move(other));
    impl = other.impl;
    other.impl = nullptr;
} // Move ctor Array

Array::~Array()
{
    delete impl;
} // Dtor Array

Array& Array::operator=(const Array &other)
{
    resize(other);
    copyOrCast(other, impl->adim.getSize());
    return *this;
} // function Array.operator= Array

Array& Array::operator=(Array &&other)
{
    swap(std::move(other));
    std::swap(impl, other.impl);
    return *this;
} // function Array.operator= (move)

void Array::copy(const Array &other, const Type &type)
{
    auto& thisType = getType();
    auto& finalType = type.isNull() ? (thisType.isNull() ? other.getType()
                                                         : thisType )
                                    : type;
    resize(other.getDim(), finalType);
    copyOrCast(other, impl->adim.getSize());
} // function Array.copy

/** Copy a patch using an small image, a bigger one and a position.
 *
 */

void copyData(Array *big, Array *small, int x, int y, int z, bool smallToBig)
{
    auto inType = small->getType();
    auto inTypeSize = inType.getSize();
    auto outType = big->getType();
    auto outTypeSize = outType.getSize();
    auto inDim = small->getDim();
    auto outDim = big->getDim();

    ASSERT_ERROR(x + inDim.x > outDim.x || y + inDim.y > outDim.y,
                 "Input image dimensions should fit inside the output image "
                         "from the starting position for the copy.");

    ASSERT_ERROR(inType.isNull() || outType.isNull(),
                 "Both images should have non null type.");

    auto n = inDim.x;
    auto inJump = n * inTypeSize;
    auto outJump = outDim.x * outTypeSize;
    auto inData = small->getDataAsChar();
    auto outData = big->getDataAsChar() + (x * outTypeSize + y * outJump);

    if (!smallToBig)  // Change order of input/output
    {
        std::swap(inData, outData);
        std::swap(inType, outType);
        std::swap(inJump, outJump);
    }


    for (size_t j = 0; j < inDim.y; ++j)
    {
        if (inType == outType)
            outType.copy(inData, outData, n);
        else
            outType.operate(Type::CAST, inData, inType, outData, n);
        inData += inJump;
        outData += outJump;
    }
}

void Array::patch(const Array &input, int x, int y, int z)
{
    auto inputPtr = const_cast<Array*>(&input);
    copyData(this, inputPtr, x, y, z, true);
} // function Array.patch

void Array::extract(const Array &input, int x, int y, int z)
{
    auto inputPtr = const_cast<Array*>(&input);
    copyData(inputPtr, this, x, y, z, false);
} // function Array.extract

void Array::set(const Object &value)
{
    copyOrCast(value, impl->adim.getSize(), true);
} // function Array.set Object

Array& Array::operator+=(const Array &other)
{
    auto& dim = impl->adim;
    ASSERT_ERROR(dim != other.getDim(),
                 "Arrays should have the same dimensions.");
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
                 "Arrays should have the same dimensions.");
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
                 "Arrays should have the same dimensions.");
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
                 "Arrays should have the same dimensions.");
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
                 "Index should be between zero and the number of elements.")

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

std::ostream& emc::operator<< (std::ostream &ostream, const Array &array)
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
const T* ArrayT<T>::getData() const
{
    return GET_DATA();
}

template <class T>
ArrayDim ArrayT<T>::getDim() const
{
    return impl->adim;
} // getDim


// ================ Explicit instantiations of Templates =======================
// This allows to implement template code in the .cpp

template emc::ArrayT<int8_t > Array::getView();
template emc::ArrayT<uint8_t > Array::getView();
template emc::ArrayT<int16_t > Array::getView();
template emc::ArrayT<uint16_t > Array::getView();
template emc::ArrayT<int32_t > Array::getView();
template emc::ArrayT<uint32_t > Array::getView();
template emc::ArrayT<int64_t > Array::getView();
template emc::ArrayT<uint64_t > Array::getView();

template emc::ArrayT<float> Array::getView();
template emc::ArrayT<double> Array::getView();
template emc::ArrayT<cfloat> Array::getView();
template emc::ArrayT<cdouble> Array::getView();

template class emc::ArrayT<int8_t>;
template class emc::ArrayT<uint8_t>;
template class emc::ArrayT<int16_t>;
template class emc::ArrayT<uint16_t>;
template class emc::ArrayT<int32_t>;
template class emc::ArrayT<uint32_t>;
template class emc::ArrayT<int64_t>;
template class emc::ArrayT<uint64_t>;

template class emc::ArrayT<float>;
template class emc::ArrayT<double>;
template class emc::ArrayT<cfloat>;
template class emc::ArrayT<cdouble>;