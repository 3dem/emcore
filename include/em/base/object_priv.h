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

template <class T>
Object& Object::operator=(const T& valueIn)
{
    // Set the new type from the input value
    auto& newType = Type::get<T>();

    //FIXME: Check the logic if we want always that this adopt the other
    // type or cast its value
    allocate(newType, 1);
    const void * inputMem = &valueIn;
    auto outputMem = getPointer();
    newType.copy(inputMem, outputMem, 1);
    return *this;
}

template <class T>
Object::operator T() const
{
    // Check the type is the same of the object
    auto& type = getType();
    ASSERT_ERROR(type != Type::get<T>(), "Types are not the same");
    // Extract the value
    return *static_cast<const T *>(getPointer());
}


#endif //EM_CORE_OBJECT_PRIV_H
