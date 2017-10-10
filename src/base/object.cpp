//
// Created by josem on 12/4/16.
//

#include "em/base/object.h"

using namespace em;

Object::Object()
{
} // Ctor Object

Object::~Object()
{
    if (!typePtr->isPod())
        typePtr->destroy(valuePtr);
} // Dtor Object

ConstTypePtr Object::getType() const
{
    return typePtr;
} // function Object.getType

void Object::toStream(std::ostream &ostream) const
{

    if (typePtr->isPod())
    {
        auto valueRef = static_cast<const void *>(&valuePtr);
        typePtr->toStream(valueRef, ostream, 1);
    }
    else
        typePtr->toStream(valuePtr, ostream, 1);
} // function Object.toStream

std::ostream& em::operator<< (std::ostream &ostream, const em::Object &object)
{
    object.toStream(ostream);
    return ostream;
}