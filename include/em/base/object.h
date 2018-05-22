/**
 * @file object.h
 */

#ifndef EM_CORE_OBJECT_H
#define EM_CORE_OBJECT_H

#include <iostream>
#include <map>
#include <vector>
#include <cassert>

#include "em/base/type.h"
#include "em/base/container_priv.h"


namespace em
{
    /** @ingroup base
     *
     * Generic object to wrap underlying values.
     *
     * An object could contains any type inside.
     */
    class Object: public TypedContainer
    {
    public:
        /** Default empty constructor for an Object.
         *
         * After an Object instance is created through this constructor,
         * its Type will be the nullType and it will not contain any data.
         */
        Object() = default;

        /** Object constructor where the memory and type are provided.
         * In this case the Object will not be the "owner" of the memory
         * and should not free it when it is destroyed.
         */
        Object(const Type & type, void *memory = nullptr);

        /** Standard copy constructor.
         *
         * The newly created object will have the same data Type than the
         * other object. The underlying data will be a copy of the other's
         * data.
         */
        Object(const Object &other);

        /** Move constructor.
         * TODO: DOCUMENT!!!
         */
        Object(Object &&other) noexcept;

        /** Object class destructor. */
        // ~Object();

        /** Copy construct from an existing Object.
         *
         * The new object will take the Type inferred from the type of
         * the argument and the data will contain a copy of it.
         */
        template <class T> Object(const T &valueIn);

        /** Assign operator to store an given value.
         * The object will change its type (and do memory allocation) if
         * necessary to accommodate the new value.
         *
         * @tparam T template type of the input value.
         * @param valueIn Value that will define the internal content of the object.
         * @return Return a references to 'this' object.
         */
        template <class T> Object& operator=(const T &valueIn);
        Object& operator=(const Object &other);

        /** Set the internal value of the object.
         * The internal type will be preserved and the input value will be
         * converted if necessary.
         * @tparam T
         * @param valueIn
         */
        template <class T> void set(const T &valueIn);
        void set(const Object &other);

        /** Move assign. */
        Object& operator=(Object &&other) noexcept;

        bool operator==(const Object &other) const;
        bool operator!=(const Object &other) const;

        // Extract the value
        template<class T> T get() const;

        /** Return a View of this object. */
        Object getView();

        /** Push the value of the Object to the output stream */
        void toStream(std::ostream &ostream) const;

        /** Parse the value of the Object from the input stream */
        void fromStream(std::istream &istream);

        /** Get a string representation of the Object value */
        std::string toString() const;

        /** Parse the value of the Object from string */
        void fromString(const std::string &str);

        /** Set a new type to this object.
         * Release current memory if needed and allocate new one if needed as well.
         */
        void setType(const Type &newType);

    }; // class Object

    std::ostream& operator<< (std::ostream &ostream, const em::Object &object);

    // Some class name definitions for ease of use
    using ObjectDict = std::map<std::string, Object>;
    using ObjectVector = std::vector<Object>;


#include "object_priv.h"

} // namespace em


#endif //EM_CORE_OBJECT_H
