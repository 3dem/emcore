//
// Created by josem on 12/2/16.
//

#ifndef EM_CORE_TYPE_H
#define EM_CORE_TYPE_H

#include <typeinfo>
#include <iostream>

namespace em
{
    class TypeInfo;
    template <class T> class TypeInfoT;
    class TypeImpl;

    class Type
    {
    public:
        std::string name() const;
        std::size_t size() const;
        bool isPod() const;

        template <class T> static Type * get()
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

#include "type_priv.h"

} // namespace em

#endif //EM_CORE_TYPE_H
