
#ifndef EM_CORE_TYPE_H
#define EM_CORE_TYPE_H

#include <typeinfo>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <complex>

#include "em/base/error.h"


namespace em
{
    template <class T> class TypeImplT;

    /**
     *  \ingroup base
     *  The Type class is one of the central pieces of the em-core library.
     *  It encapsulates information about the underlying C++ datatypes
     *  (e.g. float, int8_t, double, string, etc) that are internally managed
     *  via templates. This class is implemented following some sort of
     *  Singleton pattern and this means that each instance of the same type
     *  will share the implementation and avoid data duplication. Moreover,
     *  the Type class provides several functions to manipulate chunks of memory
     *  in a generic way. This is the base for the implementation of a generic
     *  value container (Object class) or multi-elements containers (Array
     *  and Image classes).
     */
    class Type
    {
    public:
        /** Enumerate with common operations applied to most type.
         * Basic arithmetic operations can only be applied to arithmetic types.
         */
        enum Operation {
            NO_OP = 0,
            CAST = 'c',
            ADD = '+',
            SUB = '-',
            MUL = '*',
            DIV = '/',
            LOG = 'l',
            SQRT = 's'
        };

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

        /** Return True if this type is trivially copyable.
         *
         * Objects of trivially-copyable types are the only C++ objects that
         * may be safely copied with std::memcpy or serialized to/from binary
         * files with std::ofstream::write()/std::ifstream::read(). In general,
         * a trivially copyable type is any type for which the underlying bytes
         * can be copied to an array of char or unsigned char and into a new
         * object of the same type, and the resulting object would have the
         * same value as the original.
         */
         bool isTriviallyCopyable() const;

        /** Return True if this type is the NullType */
        bool isNull() const;

        /** String representation of a Type */
        std::string toString() const;

        /** Get a reference to the requested Type instance. */
        template <class T>
        static const Type& get()
        {
            static TypeImplT<T> ti;
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
         * Operate on N elements from inputMem (of type inputType) and store
         * the results into outputMem (of type of the caller type object).
         *
         * @param op Operation to be applied
         * @param inputMem Memory location of the input data
         * @param outputMem Memory where cast elements will be put
         * @param count Number of elements in both input and output
         * @param inputType The Type of the elements in inputMem
         * @param outputMem Memory where resulting elements will be stored
         * @param count Number of elements in the output
         * @param singleInput If true, the inputMem points to a single value,
         * otherwise the outputMem has the same size (count elements) of input
         */
        void operate(Operation op, const void * inputMem, const Type &inputType,
                     void * outputMem, size_t count,
                     bool singleInput=false) const;

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

        /** Swap the bytes order
         *
         * @param mem Pointer to data
         * @param count Number of data elements
         * @param typeSize Number of bytes for each element
         */
        static void swapBytes(void * mem, size_t count, size_t typeSize);

        /** Returns true if machine is little endian else false */
        static bool isLittleEndian();

        class Impl; // Implementation class that will store type information

        /** @ingroup base
         *
         * Generic memory container of a given Type.
         *
         */
        class Container
        {
        public:
            /** Default empty constructor .
             *
             * After a GenericContainer instance is created through this constructor,
             * its Type will be the nullType and it will not contain any data.
             */
            Container();

            /** Object constructor where the memory and type are provided.
             * In this case the Object will not be the "owner" of the memory
             * and should not free it when it is destroyed.
             */
            Container(const Type &type, const size_t n, void *memory=nullptr);

            /** Object class destructor. */
            virtual ~Container();

            /** Return the Type singleton instance of this object. */
            const Type & getType() const;

            /** Return a pointer to the memory where this object data is stored. */
            void * getData();
            const void * getData() const;

        protected:
            void allocate(const Type &type, const size_t n, void *memory=nullptr);
            void deallocate();

            /** Copy or cast the elements from the other Type::Container.
             * If type of current Container is not null, it will be kept, if not
             * the type of the other will be used.
             * @param other The other Type::Container
             * @param n Number of elements we want to copy
             * @param singleInput if True, it means that other Containers only
             * have a single element that will be copied or casted to this
             * Container. If false, the other Container should have the same
             * dimensions of this one.
             */
            void copyOrCast(const Container &other, size_t n,
                            bool singleInput=false);

        private:
            class Impl;
            Impl * impl;
        }; // class Container

    private:
        // Type can only be instantiated via the Type<T> static method
        Type(Impl *impl);
        // Pointer to implementation
        Impl * impl;
    };// class Type

    std::ostream& operator<< (std::ostream &ostrm, const em::Type &t);

    using cfloat = std::complex<float>;
    using cdouble = std::complex<double>;

    static const Type typeNull;
    static const Type& typeInt8= Type::get<int8_t>();
    static const Type& typeUInt8 = Type::get<uint8_t>();
    static const Type& typeInt16 = Type::get<int16_t>();
    static const Type& typeUInt16 = Type::get<uint16_t>();
    static const Type& typeInt32 = Type::get<int32_t>();
    static const Type& typeUInt32 = Type::get<uint32_t>();
    static const Type& typeInt64 = Type::get<int64_t>();
    static const Type& typeUInt64 = Type::get<uint64_t>();
    static const Type& typeSize = Type::get<size_t>();

    static const Type& typeFloat = Type::get<float>();
    static const Type& typeDouble = Type::get<double>();
    static const Type& typeCFloat = Type::get<cfloat>();
    static const Type& typeCDouble = Type::get<cdouble>();

    static const Type& typeBool = Type::get<bool>();
    static const Type& typeString = Type::get<std::string>();

    using TypeMap = std::map<int, const Type *>;

#include "type_priv.h"

} // namespace em

#endif //EM_CORE_TYPE_H
