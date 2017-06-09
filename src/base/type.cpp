//
// Created by josem on 12/2/16.
//

#include "em/base/type.h"

using namespace em;


Type::Type(TypeInfo *typeInfoPtr)
{
    implPtr = new TypeImpl();
    implPtr->typeInfoPtr = typeInfoPtr;
    implPtr->size = typeInfoPtr->size();
    implPtr->name = typeInfoPtr->name();
    implPtr->isPod = typeInfoPtr->isPod();
}

Type::~Type()
{
    delete implPtr;
}

std::string Type::name() const
{
    return implPtr->name;
}

std::size_t Type::size() const
{
    return implPtr->size;
}

bool Type::isPod() const
{
    return implPtr->isPod;
}

void Type::copy(void *inputMem, void *outputMem, size_t count) const
{
    implPtr->typeInfoPtr->copy(inputMem, outputMem, count);
}

void Type::toStream(void * inputMem, std::ostream &stream, size_t count) const
{
    implPtr->typeInfoPtr->toStream(inputMem, stream, count);
}

std::ostream& em::operator<< (std::ostream &ostrm, const Type &t)
{
    ostrm << "Type '" << t.name()
          << "' (size " << t.size()
          << ", pod " << (t.isPod() ? "true" : "false")
          << ")";
    return ostrm;
}