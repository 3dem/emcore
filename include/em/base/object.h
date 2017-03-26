/**
 * @file object.h
 */

#ifndef EM_CORE_OBJECT_H
#define EM_CORE_OBJECT_H

#include <iostream>
#include <cassert>

#include "type.h"


namespace em
{
    /** @ingroup base
     *
     * Generic object to wrap underlying values.
     *
     * An object could contains any type inside.
     */
    class Object
    {
    public:
        /** Default empty constructor for an Object.
         *
         * After an Object instance is created through this contructor,
         * the internal data is void.
         */
        Object();

        /** Object class destructor. */
        ~Object();

        /** Copy construct from an existing Object. */
        template <class T> Object(const T &valueIn);

        /** Assign operator to store an given value.
         *
         * @tparam T template type of the input value.
         * @param valueIn Value that will define the internal content of the object.
         * @return Return a references to 'this' object.
         */
        template <class T> Object& operator=(const T &valueIn);

        // Extract the value
        template<class T> operator T() const;

        /** Return the Type singleton instance of this object. */
        const Type * type() const;

    private:
        void * valuePtr = nullptr;
        Type * typePtr = nullptr;
    };

#include "object_priv.h"

}


#endif //EM_CORE_OBJECT_H
