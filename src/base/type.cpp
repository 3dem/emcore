//
// Created by josem on 12/2/16.
//

#include "em/base/type.h"

using namespace em;


Type::Type(TypeInfo *typeInfoPtr)
{
    implPtr = new TypeImpl();
    implPtr->typeInfoPtr = typeInfoPtr;
    implPtr->size = typeInfoPtr->getSize();
    implPtr->name = typeInfoPtr->getName();
    implPtr->isPod = typeInfoPtr->isPod();
}

Type::~Type()
{
    delete implPtr;
} // Dtor Type

std::string Type::getName() const
{
    return implPtr->name;
} // function Type.getName

std::size_t Type::getSize() const
{
    return implPtr->size;
} // function Type.getSize

bool Type::isPod() const
{
    return implPtr->isPod;
} // function Type.isPod

ConstTypePtr Type::inferFromString(const std::string &str)
{
    return inferFromString(str.data(), str.size());
} // function Type::inferFromString

ConstTypePtr Type::inferFromString(const char * str, size_t n)
{
    size_t dotCount = 0;
    char c;

    for (size_t i = 0; i < n; ++i)
    {
        c = str[i];
        if (!std::isdigit(c) && c != '.')
            return TypeString;

        if (c == '.')
            ++dotCount;
    }
    // At this point is should be true that the string one have digits
    // and (maybe) a dot
    if (dotCount > 1)
        return TypeString;
    else if (dotCount == 1)
        return TypeFloat; // TODO: Allow another precision by default?
    else // dotCount == 0
        return TypeInt32; //  TODO: Allow another Integer by default?
} // function Type::inferFromString

void Type::copy(const void *inputMem, void *outputMem, size_t count) const
{
    implPtr->typeInfoPtr->copy(inputMem, outputMem, count);
} // function Type.copy

void Type::cast(const void *inputMem, void *outputMem, size_t count,
                ConstTypePtr inputType) const
{
    implPtr->typeInfoPtr->cast(inputMem, outputMem, count, inputType);
} // function Type.cast

void* Type::allocate(size_t count) const
{
    return implPtr->typeInfoPtr->allocate(count);
} // function Type.destroy

void Type::deallocate(void *inputMem, size_t count) const
{
    implPtr->typeInfoPtr->deallocate(inputMem, count);
} // function Type.destroy

void Type::toStream(const void * inputMem, std::ostream &stream,
                    size_t count) const
{
    implPtr->typeInfoPtr->toStream(inputMem, stream, count);
} // function Type.toStream

void Type::fromStream(std::istream &stream, void *outputMem, size_t count) const
{
    implPtr->typeInfoPtr->fromStream(stream, outputMem, count);
} // function Type.fromStream

bool Type::equals(const void *inputMem1, const void *inputMem2,
                  size_t count) const
{
    return implPtr->typeInfoPtr->equals(inputMem1, inputMem2, count);
} // function Type.equals

std::ostream& em::operator<< (std::ostream &ostrm, const Type &t)
{
    ostrm << t.getName() << " (" << t.getSize() << " bytes)";
    return ostrm;
}