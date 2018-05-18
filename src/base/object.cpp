//
// Created by josem on 12/4/16.
//

#include <sstream>
#include "em/base/object.h"


using namespace em;


Object::Object(const Object &other)
{
    *this = other;
} // Ctor Object

Object::Object(Object &&other) noexcept
{
    swap(std::move(other));
} // Move ctor

Object::Object(const Type & type, void *memory):
        TypedContainer(type, 1, memory)
{
} // Ctor from type and memory


void Object::setType(const Type & newType)
{
    if (newType != getType())
    {
        Object o(newType);
        o.set(*this);
        swap(std::move(o));
    }
} // function Object.setType

Object Object::getView()
{
    return Object(getType(), getData());
} // function Object.getView

void Object::toStream(std::ostream &ostream) const
{
    auto& type = getType();

    if (!type.isNull())
        type.toStream(getData(), ostream, 1);
} // function Object.toStream

void Object::fromStream(std::istream &istream)
{
    auto& type = getType();
    ASSERT_ERROR(type.isNull(), "Null type object can not be parsed. ");
    type.fromStream(istream, getData(), 1);
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
    auto& type = getType();
    if (type != other.getType() || type.isNull())
        return false;

    return type.equals(getData(), other.getData(), 1);
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
    allocate(other.getType(), 1);
    copyOrCast(other, 1);
    return *this;
}

Object& Object::operator=(Object &&other) noexcept
{
    swap(std::move(other));
    return *this;
}

void Object::set(const Object &other)
{
    copyOrCast(other, 1);
} // function Object.set