//
// Created by josem on 12/6/16.
//

#ifndef EM_CORE_TYPE_PRIV_H_H
#define EM_CORE_TYPE_PRIV_H_H


//-------------- Auxiliary Type classes -----------------------


class TypeInfo
{
public:
    virtual std::string getName() const = 0;
    virtual std::size_t getSize() const = 0;
    virtual bool isPod() const = 0;

    virtual void copy(const void * inputMem, void * outputMem, size_t count) const = 0;
    virtual void destroy(void *mem) const = 0;
    virtual void cast(const void * inputMem, void * outputMem, size_t count,
                      ConstTypePtr inputType) const = 0;
    virtual void toStream(const void * inputMem, std::ostream &stream, size_t count) const = 0;
};


template <class T1, class T2>
void typeCast(const T1 * inputMem, T2 * outputMem, size_t count)
{
    std::cerr << "Invalid CAST???" << std::endl;

    THROW_ERROR("Invalid cast between types.");
};

#define TYPE_CAST_FUNC(type) template <class T1> \
void typeCast(const T1 * inputMem, type * outputMem, size_t count) { \
for (size_t i = 0; i < count; ++i, ++outputMem, ++inputMem) *outputMem = (T1) *inputMem; \
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

class TypeImpl
{
public:
    size_t size;
    std::string name;
    bool isPod;
    TypeInfo *typeInfoPtr;
};

template <class T>
class TypeInfoBase: public TypeInfo
{
public:
    TypeInfoBase() = default;

    virtual std::string getName() const override
    {
        //TODO: is this x variable used?
        T x;
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
    } // function TypeInfoBase.copy

    virtual void destroy(void *mem) const override
    {
       auto ptr = static_cast<T*>(mem);
       delete ptr;
    } // function TypeInfoBase.destroy

    virtual void cast(const void * inputMem, void * outputMem, size_t count,
                      ConstTypePtr inputType) const override
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
    } // function TypeInfoBase.cast

    virtual void toStream(const void * inputMem, std::ostream &stream,
                          size_t count) const override
    {
        auto inPtr = static_cast<const T *>(inputMem);
        for (size_t i = 0; i < count; ++i)
        {
            stream << *inPtr << " ";
            ++inPtr;
        }
    } // toStream

};


template <class T>
class TypeInfoT: public TypeInfoBase<T>
{

};

template <>
class TypeInfoT<float>: public TypeInfoBase<float>
{
public:
    virtual std::string getName() const override
    {
        return "float";
    }
};

template <>
class TypeInfoT<double>: public TypeInfoBase<double>
{
public:
    virtual std::string getName() const override
    {
        return "double";
    }
};

//------------ 8 bits signed and unsigned int ----------
template <>
class TypeInfoT<int8_t >: public TypeInfoBase<int8_t>
{
public:
    virtual std::string getName() const override
    {
        return "int8";
    }
};

template <>
class TypeInfoT<uint8_t>: public TypeInfoBase<uint8_t>
{
public:
    virtual std::string getName() const override
    {
        return "uint8";
    }
};

//------------ 16 bits signed and unsigned int ----------
template <>
class TypeInfoT<int16_t>: public TypeInfoBase<int16_t>
{
public:
    virtual std::string getName() const override
    {
        return "int16";
    }
};

template <>
class TypeInfoT<uint16_t>: public TypeInfoBase<uint16_t>
{
public:
    virtual std::string getName() const override
    {
        return "uint16";
    }
};

//------------ 32 bits signed and unsigned int ----------

template <>
class TypeInfoT<int32_t>: public TypeInfoBase<int32_t>
{
public:
    virtual std::string getName() const override
    {
        return "int32";
    }
};

template <>
class TypeInfoT<uint32_t>: public TypeInfoBase<uint32_t>
{
public:
    virtual std::string getName() const override
    {
        return "uint32";
    }
};

#endif //EM_CORE_TYPE_PRIV_H_H
