//
// Created by josem on 12/2/16.
//
#include <sstream>
#include "em/base/type.h"


using namespace em;

Type::Type()
{
    static Impl nullImp;
    impl = &nullImp;
} // Null type constructor

Type::Type(Impl *impl)
{
    this->impl = impl;
    impl->size = impl->getSize();
    impl->name = impl->getName();
    impl->ispod = impl->isPod();
    impl->id = impl->getId();
} // Type ctor based on impl

bool Type::operator==(const Type &other) const
{
    return impl->id == other.impl->id;
} // function Type.operator==

bool Type::operator!=(const Type &other) const
{
    return impl->id != other.impl->id;
} // function Type.operator!=

bool Type::operator<(const Type &other) const
{
    return impl->id < other.impl->id;
} // function Type.operator<

size_t Type::getId() const
{
    return impl->id;
} // function Type.getId

std::string Type::getName() const
{
    return impl->name;
} // function Type.getName

std::size_t Type::getSize() const
{
    return impl->size;
} // function Type.getSize

bool Type::isPod() const
{
    return impl->ispod;
} // function Type.isPod

bool Type::isTriviallyCopyable() const
{
    return impl->isTriviallyCopyable();
} // function Type.isTriviallyCopyable

bool Type::isNull() const
{
    return impl->size == 0; // The only 0-size Type is Null type
} // function Type.isNull

std::string Type::toString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
} // function toString

Type Type::inferFromString(const std::string &str)
{
    return inferFromString(str.data(), str.size());
} // function Type::inferFromString

Type Type::inferFromString(const char * str, size_t n)
{
    size_t dotCount = 0;
    char c;

    for (size_t i = 0; i < n; ++i)
    {
        c = str[i];
        if (!std::isdigit(c) && c != '.')
            return typeString;

        if (c == '.')
            ++dotCount;
    }
    // At this point is should be true that the string one have digits
    // and (maybe) a dot
    if (dotCount > 1)
        return typeString;
    else if (dotCount == 1)
        return typeFloat; // TODO: Allow another precision by default?
    else // dotCount == 0
        return typeInt32; //  TODO: Allow another Integer by default?
} // function Type::inferFromString

void Type::copy(const void *inputMem, void *outputMem, size_t count) const
{
    impl->copy(inputMem, outputMem, count);
} // function Type.copy

void Type::operate(Operation op, const void *inputMem, const Type &inputType,
                   void *outputMem, size_t count, bool singleInput) const
{
    impl->operate(op, inputMem, inputType, outputMem, count, singleInput);
} // function Type.operate

void* Type::allocate(size_t count) const
{
    return impl->allocate(count);
} // function Type.destroy

void Type::deallocate(void *inputMem, size_t count) const
{
    impl->deallocate(inputMem, count);
} // function Type.destroy

void Type::toStream(const void * inputMem, std::ostream &stream,
                    size_t count) const
{
    impl->toStream(inputMem, stream, count);
} // function Type.toStream

void Type::fromStream(std::istream &stream, void *outputMem, size_t count) const
{
    impl->fromStream(stream, outputMem, count);
} // function Type.fromStream

bool Type::equals(const void *inputMem1, const void *inputMem2,
                  size_t count) const
{
    return impl->equals(inputMem1, inputMem2, count);
} // function Type.equals

std::ostream& em::operator<< (std::ostream &ostrm, const Type &t)
{
    ostrm << "<type: " << t.getName() << ", " << t.getSize() << " bytes>";
    return ostrm;
}

void Type::swapBytes(void *mem, size_t count, size_t typeSize)
{

    size_t i = 0;

    switch (typeSize)
    {
        case 8:
        {
            auto dtmp = (uint64_t*) mem;

            for (; i < count; ++dtmp, ++i)
                *dtmp = ((*dtmp & 0x00000000000000ff) << 56) | ((*dtmp & 0xff00000000000000) >> 56) |\
                        ((*dtmp & 0x000000000000ff00) << 40) | ((*dtmp & 0x00ff000000000000) >> 40) |\
                        ((*dtmp & 0x0000000000ff0000) << 24) | ((*dtmp & 0x0000ff0000000000) >> 24) |\
                        ((*dtmp & 0x00000000ff000000) <<  8) | ((*dtmp & 0x000000ff00000000) >>  8);
        }
            break;
        case 4:
        {
            auto * dtmp = (uint32_t*) mem;

            for (; i < count; ++dtmp, ++i)
                *dtmp = ((*dtmp & 0x000000ff) << 24) | ((*dtmp & 0xff000000) >> 24) |\
                        ((*dtmp & 0x0000ff00) <<  8) | ((*dtmp & 0x00ff0000) >>  8);
        }
            break;
        case 2:
        {
            auto dtmp = (uint16_t*) mem;

            for (; i < count; ++dtmp, ++i)
                *dtmp = static_cast<uint16_t>(((*dtmp & 0x00ff) << 8) | ((*dtmp & 0xff00) >> 8));
        }
            break;

        default:
            THROW_ERROR(std::string("swapBytes: unsupported byte size "));
                       // + typeSize);
    } // switch
} // function Type::swapBytes

bool Type::isLittleEndian()
{
    static const unsigned long ul = 0x00000001;
    static bool isLE = ((int)(*((unsigned char *) &ul)))!=0;

    return isLE;
} // function Type::isLittleEndian