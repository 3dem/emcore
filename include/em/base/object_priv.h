//
// Created by josem on 12/26/16.
//

#ifndef EM_CORE_OBJECT_PRIV_H
#define EM_CORE_OBJECT_PRIV_H

// ----------------- Object implementation --------------------------


template <class T>
Object::Object(const T& value): Object()
{
    *this = value; // call operator =
}

template <class T>
Object& Object::operator=(const T& valueIn)
{
    // Set the new type from the input value
    auto newType = Type::get<T>();

    if (newType->isPod())
    {
        // Cast the void* to the specific pointer type
        auto ptr = reinterpret_cast<T *>(&valuePtr);
        *ptr = valueIn; // Store the value of the plain-old datatype
    }
    else
    {
        if (typePtr != nullptr && !typePtr->isPod())
            typePtr->destroy(valuePtr);
        T *ptr = new T(valueIn);
        valuePtr = ptr;
    }

    typePtr = newType;

    return *this;
}

template <class T>
Object::operator T() const
{
    ConstTypePtr valueTypePtr = Type::get<T>();

    // Check the type is the same of the object
    ASSERT_ERROR(typePtr != valueTypePtr, "Types are not the same");

    // Cast the void* to the specific pointer type
    auto ptr = typePtr->isPod() ? reinterpret_cast<const T*>(&valuePtr) :
               static_cast<const T*>(valuePtr);
    // Extract the value
    return *ptr;
}

#endif //EM_CORE_OBJECT_PRIV_H
