//
// Created by josem on 12/4/16.
//

#include "em/base/object.h"

using namespace em;

Object::Object()
{
} // Ctor Object

Object::Object(const Object &other)
{
    *this = other;
} // Ctor Object

Object::Object(ConstTypePtr type, void *memory):
        typePtr(type), valuePtr(memory), isPointer(true), isOwner(false)
{

} // Ctor from type and memory

Object::~Object()
{
//    std::cerr << "  - Destroying object: " << std::endl <<
//              "      Type: " << (typePtr == nullptr ? "null" : typePtr->getName()) << std::endl <<
//              "     Value: " << *this << std::endl;
    // Release old memory if necessary
    if (isPointer and isOwner and typePtr != nullptr)
    {
//        std::cerr << "    Memory: " << valuePtr << std::endl;
        typePtr->deallocate(valuePtr, 1);
    }
} // Dtor Object

ConstTypePtr Object::getType() const
{
    return typePtr;
} // function Object.getType

void Object::setType(ConstTypePtr newType)
{
    // Release old memory if necessary
    if (isPointer and isOwner and typePtr != nullptr)
        typePtr->deallocate(valuePtr, 1);

    typePtr = newType;
    // Set a pointer and allocated memory if new type is not POD
    if ((isPointer = isOwner = !typePtr->isPod()))
        valuePtr = typePtr->allocate(1);
}

void Object::toStream(std::ostream &ostream) const
{
    if (typePtr == nullptr)
    {
        ostream << "None";
        return;
    }
    if (typePtr->isPod())
    {
        auto valueRef = static_cast<const void *>(&valuePtr);
        typePtr->toStream(valueRef, ostream, 1);
    }
    else
        typePtr->toStream(valuePtr, ostream, 1);
} // function Object.toStream

bool Object::operator==(const Object &other) const
{
    if (typePtr != other.typePtr || typePtr == nullptr)
        return false;

    return typePtr->equals(getPointer(), other.getPointer(), 1);
} // function Object.operator==

bool Object::operator!=(const Object &other) const
{
   return ! (*this == other);
} // function Object.operator!=

std::ostream& em::operator<< (std::ostream &ostream, const em::Object &object)
{
    object.toStream(ostream);
    return ostream;
}

Object& Object::operator=(const Object &other)
{
    setType(other.getType());
    typePtr->copy(other.getPointer(), getPointer(), 1);
    return *this;
}