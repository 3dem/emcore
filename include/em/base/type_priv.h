//
// Created by josem on 12/6/16.
//

#ifndef EM_CORE_TYPE_PRIV_H_H
#define EM_CORE_TYPE_PRIV_H_H

//-------------- Auxiliary Type classes -----------------------

class TypeInfo
{
public:
    virtual std::string name() const = 0;
    virtual std::size_t size() const = 0;
    virtual void copy(void * inputMem, void * outputMem, size_t count) const = 0;
    virtual bool isPod() const = 0;
};


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

    virtual std::string name() const
    {
        T x;
        return typeid(T).name();
    }

    virtual std::size_t size() const
    {
        return sizeof(T);
    }

    virtual bool isPod() const
    {
        return std::is_pod<T>();
    }

    virtual void copy(void * inputMem, void * outputMem, size_t count) const
    {
        if (isPod())
            memcpy(outputMem, inputMem, count * size());
        else
        {
            T * inPtr = static_cast<T *>(inputMem);
            T * outPtr = static_cast<T *>(outputMem);
            for (size_t i = 0; i < count; ++i)
            {
                *outPtr = *inPtr; // copy objects
                ++inPtr;
                ++outPtr;
            }
        }


    }

};


template <class T>
class TypeInfoT: public TypeInfoBase<T>
{

};


template <>
class TypeInfoT<double>: public TypeInfoBase<double>
{
public:
    virtual std::string name() const { return "double"; }
};

template <>
class TypeInfoT<float>: public TypeInfoBase<float>
{
public:
    virtual std::string name() const { return "float"; }
};


#endif //EM_CORE_TYPE_PRIV_H_H
