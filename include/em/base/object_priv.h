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
    ASSERT_ERROR(isView(), "View objects can not be assigned. ");
    // Change type and allocate memory if necessary
    allocate(Type::get<T>(), 1);
    set(valueIn);
    return *this;
} // function Object::operator=

template <class T>
void Object::set(const T& valueIn)
{
    auto& type = getType();
    auto& valueType = Type::get<T>();

    if (type.isNull())
        allocate(valueType, 1);
    // Set the new type from the input value
    const void * inputMem = &valueIn;
    auto outputMem = getData();
    if (type == valueType)
        type.copy(inputMem, outputMem, 1);
    else
        type.operate(Type::CAST, inputMem, valueType, outputMem, 1);
} // function Object.set

template <class T>
Object::operator T() const
{
    // Check the type is the same of the object
    auto& type = getType();
    auto& otherType = Type::get<T>();

    if (type == otherType)
        return *static_cast<const T *>(getData());

    T value;
    otherType.operate(Type::CAST, getData(), getType(), &value, 1);
    return value;
}


#endif //EM_CORE_OBJECT_PRIV_H
