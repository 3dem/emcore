//
// Created by josem on 12/4/16.
//

#include "em/base/object.h"

using namespace em;

Object::Object()
{
    valuePtr = malloc(8); // allocate enough memory for basic types and pointers
}

Object::~Object()
{
    free(valuePtr);
}

ConstTypePtr Object::getType() const
{
    return typePtr;
}

void Object::toStream(std::ostream &ostream) const
{
    typePtr->toStream(valuePtr, ostream, 1);
    else
        typePtr->toStream(*valuePtr, ostream, 1);
}

std::ostream& em::operator<< (std::ostream &ostream, const em::Object &object)
{
    object.toStream(ostream);
    return ostream;
}