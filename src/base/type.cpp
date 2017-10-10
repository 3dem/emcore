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

void Type::copy(const void *inputMem, void *outputMem, size_t count) const
{
    implPtr->typeInfoPtr->copy(inputMem, outputMem, count);
} // function Type.copy

void Type::destroy(void *inputMem) const
{
    implPtr->typeInfoPtr->destroy(inputMem);
} // function Type.destroy

void Type::toStream(const void * inputMem, std::ostream &stream, size_t count) const
{
    implPtr->typeInfoPtr->toStream(inputMem, stream, count);
} // function Type.toStream

std::ostream& em::operator<< (std::ostream &ostrm, const Type &t)
{
    ostrm << t.getName() << " (" << t.getSize() << " bytes)";
    return ostrm;
}