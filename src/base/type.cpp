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

void Type::castFrom(const void *inputMem, void *outputMem, size_t count,
                    ConstTypePtr inputType) const
{
    implPtr->typeInfoPtr->castFrom(inputMem, outputMem, count, inputType);
} // function Type.cast

void Type::castTo(const void *inputMem, void *outputMem, size_t count,
                ConstTypePtr outputType) const
{
    implPtr->typeInfoPtr->castTo(inputMem, outputMem, count, outputType);
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

void em::swapBytes(void *mem, size_t count, size_t typeSize)
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
            THROW_ERROR("swapBytes: unsupported byte size " + typeSize);
    }
}
