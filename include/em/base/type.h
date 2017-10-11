
#ifndef EM_CORE_TYPE_H
#define EM_CORE_TYPE_H

#include <typeinfo>
#include <iostream>
#include <cstring>

#include "em/base/error.h"


namespace em
{
    class TypeInfo;
    template <class T> class TypeInfoT;
    class TypeImpl;
    class Type;

    using ConstTypePtr = const Type *;

    /**
     *  \ingroup base
     *  The Type class provides extended information about system types.
     *
     *  This class can not be instantiated and only singleton instances
     *
     */
    class Type
    {
    public:
        /** Return the name of the type */
        std::string getName() const;
        /** Return the size in bytes of this type */
        std::size_t getSize() const;
        /** Return True if this type is a plain old type (POD) */
        bool isPod() const;

        /** Get instances of given types.
         *
         * @return Returns a pointer to the singleton Type instance.
         */
        template <class T> static ConstTypePtr get()
        {
            static TypeInfoT<T> ti;
            static Type t(&ti);
            return &t;
        }

        /** Copy n elements from inputMem to outputMem assuming both
         * memory locations point to data of this Type.
         *
         * This function is useful for memory manipulation from generic
         * memory container such as Array or Image where they store the
         * pointer to the data and know the underlying data type.
         *
         * @param inputMem Memory location of the input data
         * @param outputMem Memory location of the output data
         * @param count Number of elements that are in inputMem
         */
        void copy(const void * inputMem, void * outputMem, size_t count) const;

        /**
         * Cast n elements from inputMem (of type inputType) into outputMem
         * (of type of the caller type object).
         * @param inputMem Memory location of the input data
         * @param outputMem Memory where cast elements will be put
         * @param count Number of elements in both input and output
         * @param inputType The Type of the elements in inputMem
         */
        void cast(const void * inputMem, void * outputMem, size_t count,
                  ConstTypePtr inputType) const;

        void destroy(void * inputMem) const;

        void toStream(const void * inputMem, std::ostream &stream, size_t count) const;

    private:
        // Type can only be instantiated via the Type::get<T> static method
        Type(TypeInfo *typeInfoPtr);
        ~Type();
        // Pointer to implementation
        TypeImpl * implPtr;
    };// class Type

    std::ostream& operator<< (std::ostream &ostrm, const em::Type &t);

    ConstTypePtr const TypeInt8 = Type::get<int8_t>();
    ConstTypePtr const TypeUInt8 = Type::get<uint8_t>();
    ConstTypePtr const TypeInt16 = Type::get<int16_t>();
    ConstTypePtr const TypeUInt16 = Type::get<uint16_t>();
    ConstTypePtr const TypeInt32 = Type::get<int32_t>();
    ConstTypePtr const TypeUInt32 = Type::get<uint32_t>();
    ConstTypePtr const TypeFloat = Type::get<float>();
    ConstTypePtr const TypeDouble = Type::get<double>();



#include "type_priv.h"

} // namespace em

#endif //EM_CORE_TYPE_H
