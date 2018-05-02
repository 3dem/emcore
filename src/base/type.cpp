//
// Created by josem on 12/2/16.
//
#include <sstream>
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
    impl->id = impl->getId();

    if (impl->name == "int8")
        std::cout << "Type::Type: " << *this << " id: " << getId() << std::endl;

} // Type ctor based on impl

bool Type::operator==(const Type &other) const
{
    return impl->id == other.impl->id;
} // function Type.operator==

bool Type::operator!=(const Type &other) const
{
    return impl->id != other.impl->id;
} // function Type.operator!=

bool Type::operator<(const Type &other) const
{
    return impl->id < other.impl->id;
} // function Type.operator<

size_t Type::getId() const
{
    return impl->id;
} // function Type.getId

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

bool Type::isTriviallyCopyable() const
{
    return impl->isTriviallyCopyable();
} // function Type.isTriviallyCopyable

bool Type::isNull() const
{
    return impl->size == 0; // The only 0-size Type is Null type
} // function Type.isNull

std::string Type::toString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
} // function toString

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

void Type::operate(Operation op, const void *inputMem, const Type &inputType,
                   void *outputMem, size_t count, bool singleInput) const
{
    impl->operate(op, inputMem, inputType, outputMem, count, singleInput);
} // function Type.operate

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
    ostrm << "(type: " << t.getName() << ", " << t.getSize() << " bytes)";
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

const void* Type::Container::getData() const { return impl->data; }

void* Type::Container::getData() { return impl->data; }

void Type::Container::allocate(const Type &type, const size_t n, void *memory)
{
    // If the type have the same size and we are going to allocate the
    // same number of elements, then we will use the same amount of
    // memory, so there is not need for a new allocation if we own the memory
    if (impl->data != nullptr && impl->size > 0 && type.isTriviallyCopyable()
        && impl->size * impl->type.getSize() == n * type.getSize())
    {
        impl->type = type; // set new type and return, not allocation needed
        return;
    }

    deallocate(); // deallocate using previous type
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

/** Copy or cast the elements from the other Type::Container.
 * If type of current Container is not null, it will be kept, if not
 * the type of the other will be used.
 * @param other The other Type::Container
 * @param n Number of elements we want to copy
 */
void Type::Container::copyOrCast(const Type::Container &other, size_t n,
                                 bool singleInput)
{
    auto& otherType = other.getType();
    auto& finalType = impl->type.isNull() ? otherType: impl->type;

    allocate(finalType, n);

    if (finalType == otherType && !singleInput)
        finalType.copy(other.getData(), impl->data, n);
    else
        finalType.operate(Type::CAST, other.getData(), otherType, impl->data,
                          n, singleInput);
} // function Container.copyOrCast

void Type::swapBytes(void *mem, size_t count, size_t typeSize)
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
            THROW_ERROR(std::string("swapBytes: unsupported byte size "));
                       // + typeSize);
    } // switch
} // function Type::swapBytes

bool Type::isLittleEndian()
{
    static const unsigned long ul = 0x00000001;
    static bool isLE = ((int)(*((unsigned char *) &ul)))!=0;

    return isLE;
} // function Type::isLittleEndian