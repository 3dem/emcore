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
    auto& valueType = Type::get<T>();
    // Only change to the new type when the current type is null
    if (getType().isNull())
    {
        allocate(valueType, 1);
    }
    auto& type = getType();
    const void * inputMem = &valueIn;
    auto outputMem = getData();
    if (type == valueType)
        type.copy(inputMem, outputMem, 1);
    else
        type.cast(inputMem, outputMem, 1, valueType);
    return *this;
}

template <class T>
Object::operator T() const
{
    // Check the type is the same of the object
    auto& type = getType();
    ASSERT_ERROR(type != Type::get<T>(), "Types are not the same");
    // Extract the value
    return *static_cast<const T *>(getData());
}


#endif //EM_CORE_OBJECT_PRIV_H
