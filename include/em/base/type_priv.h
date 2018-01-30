//
// Created by josem on 12/6/16.
//

#ifndef EM_CORE_TYPE_PRIV_H_H
#define EM_CORE_TYPE_PRIV_H_H


//-------------- Auxiliary Type classes -----------------------


class Type::Impl
{
public:
    virtual std::string getName() const = 0;
    virtual std::size_t getSize() const = 0;
    virtual bool isPod() const = 0;

    virtual void copy(const void *inputMem, void *outputMem,
                      size_t count) const = 0;
    virtual void * allocate(size_t count) const = 0;
    virtual void deallocate(void *mem, size_t count) const = 0;
    virtual void cast(const void * inputMem, void * outputMem, size_t count,
                      const Type &inputType) const = 0;
    virtual void toStream(const void * inputMem,
                          std::ostream &stream, size_t count) const = 0;
    virtual void fromStream(std::istream &stream, void *outputMem,
                            size_t count) const = 0;
    virtual bool equals(const void *inputMem1,
                        const void *inputMem2, size_t count) const = 0;

    size_t size;
    std::string name;
    bool ispod;
};


template <class T1, class T2>
void typeCast(const T1 * inputMem, T2 * outputMem, size_t count)
{
    std::cerr << "Invalid CAST???" << std::endl;

    THROW_ERROR("Invalid cast between types.");
};

#define TYPE_CAST_FUNC(type) template <class T1> \
void typeCast(const T1 * inputMem, type * outputMem, size_t count) { \
for (size_t i = 0; i < count; ++i, ++outputMem, ++inputMem) *outputMem = (type) *inputMem; \
}

TYPE_CAST_FUNC(int8_t);
TYPE_CAST_FUNC(uint8_t);
TYPE_CAST_FUNC(int16_t);
TYPE_CAST_FUNC(uint16_t);
TYPE_CAST_FUNC(int32_t);
TYPE_CAST_FUNC(uint32_t);
TYPE_CAST_FUNC(float);
TYPE_CAST_FUNC(double);

#undef TYPE_CAST_FUNC


template <class T>
class TypeImplBaseT: public Type::Impl
{
public:
    TypeImplBaseT() = default;

    virtual std::string getName() const override
    {
        return typeid(T).name();
    }

    virtual std::size_t getSize() const override
    {
        return sizeof(T);
    }

    virtual bool isPod() const override
    {
        return std::is_pod<T>();
    }

    virtual void copy(const void * inputMem, void * outputMem,
                      size_t count) const override
    {
        if (isPod())
            memcpy(outputMem, inputMem, count * getSize());
        else
        {
            auto inPtr = static_cast<const T *>(inputMem);
            T * outPtr = static_cast<T *>(outputMem);
            for (size_t i = 0; i < count; ++i)
            {
                *outPtr = *inPtr; // copy objects
                ++inPtr;
                ++outPtr;
            }
        }
    } // function TypeImplBaseT.copy

    virtual void * allocate(size_t count) const override
    {
        if (count > 1)
            return new T[count];
        else
            return new T;
    } // function TypeImplBaseT.destroy

    virtual void deallocate(void *mem, size_t count) const override
    {
        auto ptr = static_cast<T*>(mem);
        if (count > 1)
            delete [] ptr;
        else
            delete ptr;
    } // function TypeImplBaseT.destroy

    virtual void cast(const void * inputMem, void * outputMem, size_t count,
                      const Type &inputType) const override
    {
        auto outputMemT = static_cast<T *>(outputMem);

#define CAST_IF_TYPE(type) if (inputType == Type::get<type>()) typeCast(static_cast<const type*>(inputMem), outputMemT, count)

        CAST_IF_TYPE(int8_t);
        CAST_IF_TYPE(uint8_t);
        CAST_IF_TYPE(int16_t);
        CAST_IF_TYPE(uint16_t);
        CAST_IF_TYPE(int32_t);
        CAST_IF_TYPE(uint32_t);
        CAST_IF_TYPE(float);
        CAST_IF_TYPE(double);
    } // function TypeImplBaseT.cast

    virtual void toStream(const void * inputMem, std::ostream &stream,
                          size_t count) const override
    {
        auto inPtr = static_cast<const T *>(inputMem);

        if (count == 1)
            stream << *inPtr;
        else
        {
            for (size_t i = 0; i < count; ++i)
            {
                stream << *inPtr << " ";
                ++inPtr;
            }
        }
    } // function toStream

    virtual void fromStream(std::istream &stream, void *outputMem,
                            size_t count) const override
    {
        auto outputMemT = static_cast<T *>(outputMem);
        for (size_t i = 0; i < count; ++i)
        {
            stream >> *outputMemT;
            ++outputMemT;
        }
    } // function fromStream

    virtual bool equals(const void *inputMem1, const void *inputMem2,
                        size_t count) const override
    {
        auto ptr1 = static_cast<const T *>(inputMem1);
        auto ptr2 = static_cast<const T *>(inputMem2);

        for (size_t i = 0; i < count; ++i, ++ptr1, ++ptr2)
            if (*ptr1 != *ptr2)
                return false;
        return true;
    } // function equals

};


template <class T>
class TypeImplT: public TypeImplBaseT<T>
{

};

#define DEFINE_TYPENAME(type, name) \
template <> class TypeImplT<type>: public TypeImplBaseT<type> { \
public: \
    virtual std::string getName() const override { return name; } \
}

DEFINE_TYPENAME(float, "float");
DEFINE_TYPENAME(double, "double");
DEFINE_TYPENAME(int8_t, "int8");
DEFINE_TYPENAME(uint8_t, "uint8");
DEFINE_TYPENAME(int16_t, "int16");
DEFINE_TYPENAME(uint16_t, "uint16");
DEFINE_TYPENAME(int32_t, "int32");
DEFINE_TYPENAME(uint32_t, "uint32");

DEFINE_TYPENAME(std::string, "string");

#undef DEFINE_TYPENAME

#endif //EM_CORE_TYPE_PRIV_H_H
