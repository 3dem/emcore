//
// Created by josem on 12/6/16.
//

#ifndef EM_CORE_TYPE_PRIV_H_H
#define EM_CORE_TYPE_PRIV_H_H


//-------------- Auxiliary Type classes -----------------------

/** Base class for internal Type implementation.
 * By default, this class will correspond to the Null type instance
 * and will raise an error on most operations. These methods
 * should be overriden in subclasses of real types.
 */
class Type::Impl
{
#define NOT_IMPLEMENTED { THROW_ERROR("Operation not valid for Null type."); }

public:
    virtual std::string getName() const { return "null"; }
    virtual std::size_t getSize() const {return 0;}
    virtual bool isPod() const {return false; }

    virtual void copy(const void *inputMem, void *outputMem,
                      size_t count) const NOT_IMPLEMENTED;
    virtual void * allocate(size_t count) const NOT_IMPLEMENTED;
    virtual void deallocate(void *mem,
                            size_t count) const NOT_IMPLEMENTED;
    virtual void operate(Type::Operation op, const void * inputMem,
                         const Type &inputType, void * outputMem,
                         size_t count, bool singleInput) const NOT_IMPLEMENTED;
    virtual void toStream(const void * inputMem,
                          std::ostream &stream,
                          size_t count) const NOT_IMPLEMENTED;
    virtual void fromStream(std::istream &stream,
                            void *outputMem,
                            size_t count) const NOT_IMPLEMENTED;
    virtual bool equals(const void *inputMem1,
                        const void *inputMem2,
                        size_t count) const NOT_IMPLEMENTED;

    size_t size;
    std::string name;
    bool ispod;

#undef NOT_IMPLEMENTED
};

// Implement a shortcut trait to check if two types are numeric
template<class T1, class T2>
struct both_arithmetic{
    static const bool value = std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value;
};


// Implementation of some operations when the types are numeric
// and the operation make sense
template <bool B>
class TypeOperator
{
public:
    template <class T1, class T2>
    static void operate(Type::Operation op, const T1 * inputMem, T2 * outputMem,
                        size_t count, bool singleInput)
    {
        if (singleInput)
        {
            T2 value = static_cast<T2>(*inputMem);

#define OP_SINGLE(_op) for (size_t i = 0; i < count; ++i, ++outputMem) *outputMem _op value; break

            switch (op)
            {
                case Type::CAST: OP_SINGLE(=);
                case Type::ADD: OP_SINGLE(+=);
                case Type::SUB: OP_SINGLE(-=);
                case Type::MUL: OP_SINGLE(*=);
                case Type::DIV: OP_SINGLE(/=);
                default:
                    THROW_ERROR("Operation not supported!");
            }
#undef OP_SINGLE
        }
        else
        {
# define OP_MULT(_op) for (size_t i = 0; i < count; ++i, ++outputMem, ++inputMem) *outputMem _op static_cast<T2>(*inputMem); break
            switch (op)
            {
                case Type::CAST: OP_MULT(=);
                case Type::ADD: OP_MULT(+=);
                case Type::SUB: OP_MULT(-=);
                case Type::MUL: OP_MULT(*=);
                case Type::DIV: OP_MULT(/=);
                default:
                    THROW_ERROR("Operation not supported!");
            }
#undef OP_MULT
        }

    }
};

// Just default implementation to cover other types (e.g, string, char *, etc)
// that do not support the operations
template <>
class TypeOperator<false>
{
public:
    template <class T1, class T2>
    static void operate(Type::Operation op, const T1 * inputMem, T2 * outputMem,
                        size_t count, bool singleInput)
    {
        THROW_ERROR("Invalid cast between types.");
    }
};

// Implement stream operations for enums since they are needed in toStream
// and fromStream generic implementations in Type
template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

template<typename T>
std::istream& operator>>(typename std::enable_if<std::is_enum<T>::value, std::istream>::type& stream, T& e)
{
    typename std::underlying_type<T>::type v;
    stream >> v;
    e = static_cast<T>(v);
    return stream;
}

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

    virtual void operate(Type::Operation op, const void * inputMem, const Type &inputType,
                 void * outputMem, size_t count, bool singleInput) const override
    {
        auto outputMemT = static_cast<T *>(outputMem);
        // std::is_arithmetic<type>::value
#define CAST_IF_TYPE(type) if (inputType == Type::get<type>()) \
        TypeOperator<both_arithmetic<T, type>::value>::operate(op, static_cast<const type*>(inputMem), outputMemT, count, singleInput);

        CAST_IF_TYPE(int8_t);
        CAST_IF_TYPE(uint8_t);
        CAST_IF_TYPE(int16_t);
        CAST_IF_TYPE(uint16_t);
        CAST_IF_TYPE(int32_t);
        CAST_IF_TYPE(uint32_t);
        CAST_IF_TYPE(int64_t);
        CAST_IF_TYPE(uint64_t);
        CAST_IF_TYPE(size_t);

        CAST_IF_TYPE(float);
        CAST_IF_TYPE(double);

        CAST_IF_TYPE(bool);

#undef CAST_IF_TYPE
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
DEFINE_TYPENAME(int64_t, "int64");
DEFINE_TYPENAME(uint64_t, "uint64");
DEFINE_TYPENAME(size_t, "size_t");


DEFINE_TYPENAME(bool, "bool");
DEFINE_TYPENAME(std::string, "string");

#undef DEFINE_TYPENAME

#endif //EM_CORE_TYPE_PRIV_H_H
