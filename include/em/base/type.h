
#ifndef EM_CORE_TYPE_H
#define EM_CORE_TYPE_H

#include <typeinfo>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>

#include "em/base/error.h"


namespace em
{
    template <class T> class TypeInfoT;

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
        class Impl; // Implementation class that will store type information

    public:
        /** Empty constructor, Null type */
        Type();

        bool operator==(const Type &other) const;
        bool operator!=(const Type &other) const;

        /** Return the name of the type */
        std::string getName() const;

        /** Return the size in bytes of this type */
        std::size_t getSize() const;

        /** Return True if this type is a plain old type (POD) */
        bool isPod() const;

        /** Return True if this type is the NullType */
        bool isNull() const;

        /** Get instances of given types.
         *
         * @return Returns a pointer to the singleton Type instance.
         */
        template <class T>
        static const Type& get()
        {
            static TypeInfoT<T> ti;
            static Type t(&ti);
            return t;
        }

        /** Infer a Type from a string literal.
         * It will check first if it has non-numeric characters in which
         * case the type will be TypeString. If it is numeric, it will be
         * differentiated between Integer and Double.
         *
         * @return The inferred Type from the string literal
         */
        static Type inferFromString(const std::string &str);

        /** Infer a Type from a char * taking into account n first characters.
         * This function is useful when parsing multiple values from a string.
         *
         * @param str Input string as char *
         * @param n Number of characters to take into account
         * @return The inferred Type from the string literal
         */
        static Type inferFromString(const char * str, size_t n);

        /** Copy N elements from inputMem to outputMem assuming both
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
         * Cast N elements from inputMem (of type inputType) into outputMem
         * (of type of the caller type object).
         * @param inputMem Memory location of the input data
         * @param outputMem Memory where cast elements will be put
         * @param count Number of elements in both input and output
         * @param inputType The Type of the elements in inputMem
         */
        void cast(const void * inputMem, void * outputMem, size_t count,
                  const Type &inputType) const;

        /**
         * Allocate memory for N elements of this Type.
         * @param count Number of elements to be allocated
         * @return The pointer to the allocated memory
         */
        void * allocate(size_t count) const;

        /**
         * Release the memory allocated for N elements of this Type.
         * @param inputMem Pointer to allocated memory location.
         * @param count Number of elements that were allocated.
         */
        void deallocate(void * inputMem, size_t count) const;

        /**
         * Push N elements of this Type to an output stream.
         * @param inputMem Pointer to the memory location of the elements.
         * @param stream Output stream.
         * @param count Number of elements.
         */
        void toStream(const void * inputMem, std::ostream &stream,
                      size_t count) const;

        /**
         * Read N elements of this Type from an input stream.
         * @param stream Input stream.
         * @param outputMem Memory location where to put read elements.
         * @param count Number of elements to read from the stream.
         */
        void fromStream(std::istream &stream, void *outputMem,
                        size_t count) const;

        bool equals(const void *inputMem1, const void *inputMem2,
                    size_t count) const;

    private:
        // Type can only be instantiated via the Type<T> static method
        Type(Impl *impl);
        // Pointer to implementation
        Impl * impl;
    };// class Type

    std::ostream& operator<< (std::ostream &ostrm, const em::Type &t);

    static const Type TypeNull;
    static const Type TypeInt8= Type::get<int8_t>();
    static const Type TypeUInt8 = Type::get<uint8_t>();
    static const Type TypeInt16 = Type::get<int16_t>();
    static const Type TypeUInt16 = Type::get<uint16_t>();
    static const Type TypeInt32 = Type::get<int32_t>();
    static const Type TypeUInt32 = Type::get<uint32_t>();
    static const Type TypeFloat = Type::get<float>();
    static const Type TypeDouble = Type::get<double>();

    static const Type TypeString = Type::get<std::string>();

    using TypeMap = std::map<int, const Type *>;


#include "type_priv.h"

} // namespace em

#endif //EM_CORE_TYPE_H
