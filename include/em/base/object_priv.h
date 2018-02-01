//
// Created by josem on 12/26/16.
//

#ifndef EM_CORE_OBJECT_PRIV_H
#define EM_CORE_OBJECT_PRIV_H

// ----------------- Object implementation --------------------------




template <class T>
Object::Object(const T& value)
{
    *this = value; // call operator =
}

void * Object::getPointer()
{
    return isPointer ? valuePtr : reinterpret_cast<void *>(&valuePtr);
}

const void * Object::getPointer() const
{
    return isPointer ? valuePtr : reinterpret_cast<const void *>(&valuePtr);
}


template <class T>
Object& Object::operator=(const T& valueIn)
{
    // Set the new type from the input value
    auto newType = Type::get<T>();
    const void * inputMem = &valueIn;

    setType(newType);
    type.copy(inputMem, getPointer(), 1);
    return *this;
}


template <class T>
Object::operator T() const
{
    // Check the type is the same of the object
    ASSERT_ERROR(type != Type::get<T>(), "Types are not the same");

    // Cast the void* to the specific pointer type
    auto ptr = type.isPod() ? reinterpret_cast<const T*>(&valuePtr) :
               static_cast<const T*>(valuePtr);
    // Extract the value
    return *ptr;
}


#endif //EM_CORE_OBJECT_PRIV_H
