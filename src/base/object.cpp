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

const Type * Object::type() const
{
    return typePtr;
}