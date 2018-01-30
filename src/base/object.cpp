//
// Created by josem on 12/4/16.
//

#include <sstream>
#include "em/base/object.h"

using namespace em;

Object::Object()
{
} // Ctor Object

Object::Object(const Object &other)
{
    *this = other;
} // Ctor Object

Object::Object(const Type & type, void *memory):
        type(type), valuePtr(memory), isPointer(true), isOwner(false)
{

} // Ctor from type and memory

Object::~Object()
{
//    std::cerr << "  - Destroying object: " << std::endl <<
//              "      Type: " << (typePtr == nullptr ? "null" : typePtr->getName()) << std::endl <<
//              "     Value: " << *this << std::endl;
    // Release old memory if necessary
    if (isPointer && isOwner && !type.isNull())
    {
//        std::cerr << "    Memory: " << valuePtr << std::endl;
        type.deallocate(valuePtr, 1);
    }
} // Dtor Object

const Type & Object::getType() const
{
    return type;
} // function Object.getType

void Object::setType(const Type & newType)
{
    //TODO: Check what happens when newType.isNull() == True
    // Release old memory if necessary
    if (isPointer && isOwner && !type.isNull())
        type.deallocate(valuePtr, 1);

    type = newType;
    // Set a pointer and allocated memory if new type is not POD
    if ((isPointer = isOwner = !type.isPod()))
        valuePtr = type.allocate(1);
}

void Object::toStream(std::ostream &ostream) const
{
    if (!type.isNull())
        type.toStream(getPointer(), ostream, 1);
} // function Object.toStream

void Object::fromStream(std::istream &istream)
{
    ASSERT_ERROR(type.isNull(), "Null type object can not be parsed. ");

    type.fromStream(istream, getPointer(), 1);
} // function Object.fromStream

std::string Object::toString() const
{
    std::stringstream ss;
    toStream(ss);

    return ss.str();
} // function Object.toString

void Object::fromString(const std::string &str)
{
    std::stringstream ss(str);
    fromStream(ss);
} // function Object.fromString

bool Object::operator==(const Object &other) const
{
    if (type != other.type || type.isNull())
        return false;

    return type.equals(getPointer(), other.getPointer(), 1);
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
    type.copy(other.getPointer(), getPointer(), 1);
    return *this;
}