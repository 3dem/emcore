
#ifndef EM_CORE_TYPE_H
#define EM_CORE_TYPE_H

#include <typeinfo>
#include <iostream>

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
        std::string name() const;
        /** Return the size in bytes of this type */
        std::size_t size() const;
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

    private:
        // Type can only be instantiated via the Type::get<T> static method
        Type(TypeInfo *typeInfoPtr);
        ~Type();
        // Pointer to implementation
        TypeImpl * implPtr;
    };// class Type

    std::ostream& operator<< (std::ostream &ostrm, const em::Type &t);

    ConstTypePtr const TypeFloat = Type::get<float>();
    ConstTypePtr const TypeInt = Type::get<int>();
    ConstTypePtr const TypeDouble = Type::get<double>();

#include "type_priv.h"

} // namespace em

#endif //EM_CORE_TYPE_H
