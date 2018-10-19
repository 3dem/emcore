//
// Created by josem on 5/8/18.
//

#ifndef EM_CORE_CONTAINER_PRIV_H
#define EM_CORE_CONTAINER_PRIV_H

#include "em/base/type.h"


namespace em
{
/** @ingroup base
 *
 * Generic memory container of a given Type.
 *
 */
    class TypedContainer
    {
    public:
        /** Default empty constructor .
         *
         * After a GenericContainer instance is created through this constructor,
         * its Type will be the nullType and it will not contain any data.
         */
        TypedContainer() = default;

        /** Object constructor where the memory and type are provided.
         * In this case the Object will not be the "owner" of the memory
         * and should not free it when it is destroyed.
         */
        TypedContainer(const Type &type, const size_t n, void *memory = nullptr)
        {
            allocate(type, n, memory);
        }

        /** Object class destructor. */
        virtual ~TypedContainer() { deallocate(); }

        /** Return the Type singleton instance of this object. */
        inline const Type &getType() const { return type; }

        /** Return a pointer to the memory where this object data is stored. */
        inline void *getData() { return data; }

        inline const void *getData() const { return data; }

        /** Return True if this container is a View, i.e, it does not own
         * the memory but points to some existing memory location.
         */
        inline bool isView() const { return view; }

        /** Return the amount of memory that is being used. */
        inline size_t getMemorySize() const { return size * type.getSize(); }

    protected:
        void allocate(const Type &type, const size_t n, void *memory = nullptr)
        {
            ASSERT_ERROR(view, "Views can not allocate memory");

            // If the type have the same size and we are going to allocate the
            // same number of elements, then we will use the same amount of
            // memory, so there is not need for a new allocation if we own the memory
            if (data != nullptr && type.isTriviallyCopyable()
                && getMemorySize() == n * type.getSize())
            {
                this->type = type; // set new type and return, not allocation needed
                return;
            }

            deallocate(); // deallocate using previous type info
            this->type = type;
            this->size = n;

            view = (memory != nullptr);
            data = (view ? memory : type.allocate(n));
        } // function allocate

        void deallocate()
        {
            if (!view && size > 0)
            {
                type.deallocate(data, size);
                size = 0;
            }
        } // function deallocate

        /** Swap function to be used from move assignment and constructor */
        inline void swap(TypedContainer &&other)
        {
            std::swap(data, other.data);
            std::swap(type, other.type);
            std::swap(size, other.size);
        }

        /** Copy or cast the elements from the other Type::TypedContainer.
         * If the type of current TypedContainer is not null, it will be kept,
         * if not the type of the other will be used.
         * @param other The other Type::TypedContainer
         * @param n Number of elements we want to copy
         * @param singleInput if True, it means that other Containers only
         * have a single element that will be copied or casted to this
         * TypedContainer. If false, the other TypedContainer should have the
         * same dimensions of this one.
         */
        void copyOrCast(const TypedContainer &other, size_t n,
                        bool singleInput = false)
        {
            auto& otherType = other.getType();
            auto& finalType = type.isNull() ? otherType: type;

            allocate(finalType, n);

            if (finalType == otherType && !singleInput)
                finalType.copy(other.getData(), data, n);
            else
                finalType.operate(Type::CAST, other.getData(), otherType, data,
                                  n, singleInput);
        } // function copyOrCast

    private:
        // Number of allocated elements in memory, if it is 0 the memory
        // is not owned by this instance and not deallocation is required
        size_t size = 0;
        void * data = nullptr;
        Type type;
        bool view = false;
    }; // class TypedContainer

} // namespace em

#endif //EM_CORE_CONTAINER_PRIV_H
