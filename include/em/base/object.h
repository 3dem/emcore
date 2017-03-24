//
// Created by josem on 12/4/16.
//

#ifndef EM_CORE_OBJECT_H
#define EM_CORE_OBJECT_H

#include <iostream>
#include <cassert>

#include "type.h"


namespace em
{
    /** Generic object to wrap underlying values.
     * An object could contains any type inside.
     */
    class Object
    {
    public:
        // Default constructor and destructor
        Object();
        ~Object();

        // Copy construct and assign operator
        template <class T> Object(const T &valueIn);
        template <class T> Object& operator=(const T &valueIn);
        // Convertion functions


        // Extract the value
        template<class T> operator T() const;

        const Type * type() const;

    private:
        void * valuePtr = nullptr;
        Type * typePtr = nullptr;
    };

#include "object_priv.h"

}


#endif //EM_CORE_OBJECT_H
