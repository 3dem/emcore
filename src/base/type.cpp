//
// Created by josem on 12/2/16.
//

#include "em/base/type.h"

using namespace em;

Type::Type()
{
    static Impl nullImp;
    impl = &nullImp;
} // Null type constructor

Type::Type(Impl *impl)
{
    this->impl = impl;
    impl->size = impl->getSize();
    impl->name = impl->getName();
    impl->ispod = impl->isPod();
} // Type ctor based on impl

bool Type::operator==(const Type &other) const
{
    return impl == other.impl;
}

bool Type::operator!=(const Type &other) const
{
    return impl != other.impl;
}

std::string Type::getName() const
{
    return impl->name;
} // function Type.getName

std::size_t Type::getSize() const
{
    return impl->size;
} // function Type.getSize

bool Type::isPod() const
{
    return impl->ispod;
} // function Type.isPod

bool Type::isNull() const
{
    return impl->size == 0; // The only 0-size Type is Null type
} // function Type.isNull

Type Type::inferFromString(const std::string &str)
{
    return inferFromString(str.data(), str.size());
} // function Type::inferFromString

Type Type::inferFromString(const char * str, size_t n)
{
    size_t dotCount = 0;
    char c;

    for (size_t i = 0; i < n; ++i)
    {
        c = str[i];
        if (!std::isdigit(c) && c != '.')
            return typeString;

        if (c == '.')
            ++dotCount;
    }
    // At this point is should be true that the string one have digits
    // and (maybe) a dot
    if (dotCount > 1)
        return typeString;
    else if (dotCount == 1)
        return typeFloat; // TODO: Allow another precision by default?
    else // dotCount == 0
        return typeInt32; //  TODO: Allow another Integer by default?
} // function Type::inferFromString

void Type::copy(const void *inputMem, void *outputMem, size_t count) const
{
    impl->copy(inputMem, outputMem, count);
} // function Type.copy

void Type::cast(const void *inputMem, void *outputMem, size_t count,
                const Type &inputType) const
{
    impl->cast(inputMem, outputMem, count, inputType);
} // function Type.cast

void* Type::allocate(size_t count) const
{
    return impl->allocate(count);
} // function Type.destroy

void Type::deallocate(void *inputMem, size_t count) const
{
    impl->deallocate(inputMem, count);
} // function Type.destroy

void Type::toStream(const void * inputMem, std::ostream &stream,
                    size_t count) const
{
    impl->toStream(inputMem, stream, count);
} // function Type.toStream

void Type::fromStream(std::istream &stream, void *outputMem, size_t count) const
{
    impl->fromStream(stream, outputMem, count);
} // function Type.fromStream

bool Type::equals(const void *inputMem1, const void *inputMem2,
                  size_t count) const
{
    return impl->equals(inputMem1, inputMem2, count);
} // function Type.equals

std::ostream& em::operator<< (std::ostream &ostrm, const Type &t)
{
    ostrm << t.getName() << " (" << t.getSize() << " bytes)";
    return ostrm;
}


// ===================== Container Implementation =======================
class Type::Container::Impl
{
public:
    // Number of allocated elements in memory, if it is 0 the memory
    // is not owned by this instance and not deallocation is required
    size_t size = 0;
    void * data = nullptr;
    Type type;
};

Type::Container::Container()
{
    impl = new Impl();
}

Type::Container::Container(const Type &type, const size_t n,
                                   void *memory):Container()
{
    allocate(type, n, memory);
} // Container Ctor

Type::Container::~Container()
{
    deallocate();
    delete impl;
} // Container Dtor

const Type& Type::Container::getType() const { return impl->type; }

const void* Type::Container::getPointer() const { return impl->data; }

void* Type::Container::getPointer() { return impl->data; }

void Type::Container::allocate(const Type &type, const size_t n, void *memory)
{
    deallocate();
    impl->type = type;

    if (memory == nullptr)
    {
        impl->size = n;
        impl->data = type.allocate(n);
    }
    else
        impl->data = memory;

} // function Container.allocate

void Type::Container::deallocate()
{
    if (impl->size)
    {
        impl->type.deallocate(impl->data, impl->size);
        impl->size = 0;
    }
} // function Container.deallocate

void Type::Container::copyOrCast(const Type &type, const size_t n,
                                  const Type::Container &other)
{
    allocate(type, n);

    if (type == other.getType())
        type.copy(other.getPointer(), impl->data, n);
    else
        type.cast(other.getPointer(), impl->data, n, other.getType());
} // function Container.copyOrCast