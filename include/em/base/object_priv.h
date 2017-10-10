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
    typePtr = Type::get<T>();

    // Cast the void* to the specific pointer type
    T *ptr = static_cast<T *>(valuePtr);

    if (typePtr->isPod())
        *ptr = valueIn; // Store the value of the plain-old datatype
    else
        ptr = const_cast<T*>(&valueIn); // Store a pointer to object types

    return *this;
}

template <class T>
Object::operator T() const
{
    ConstTypePtr valueTypePtr = Type::get<T>();

    // Check the type is the same of the object
    assert(typePtr == valueTypePtr);

    // Cast the void* to the specific pointer type
    T *ptr = static_cast<T*>(valuePtr);
    // Extract the value
    return *ptr;
}

#endif //EM_CORE_OBJECT_PRIV_H
