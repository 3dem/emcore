Basic concepts: Type and Object
===============================

Type
----

The :cpp:any:`Type` class is one of the central pieces of the emcore library.
It encapsulates information about the underlying C++ datatypes (e.g. float, int8_t, double, string, etc)
that are internally managed via templates. This class is implemented following a Singleton pattern where
each instance of the same type will share the implementation, avoiding data duplication.

Moreover, the Type class provides several functions to manipulate chunks of memory in a generic way.
This is the base for the implementation of a generic single value container (Object) or multi-elements
containers (Array and Image).

We can operate with Type instances and query basic properties such as: type name, size in bytes,
or if it is trivially copyable ot plain-old-type (POD).

.. code-block:: cpp

    using namespace emcore;

    auto f = typeFloat;
    ASSERT_EQ(f.getName(), "float");
    ASSERT_TRUE(f.isPod());
    ASSERT_EQ(f.getSize(), sizeof(float));
    std::cout << f << std::endl;

    auto d = typeDouble;
    ASSERT_EQ(d.getName(), "double");
    ASSERT_TRUE(d.isPod());
    ASSERT_EQ(d.getSize(), sizeof(double));
    std::cout << d << std::endl;

    auto i8 = typeInt8;
    ASSERT_EQ(i8.getName(), "int8");
    ASSERT_TRUE(i8.isPod());
    ASSERT_EQ(i8.getSize(), sizeof(int8_t));
    std::cout << i8 << std::endl;


The :cpp:class:`Type` also contains some utility functions, such as type inference from a given
string value. This function is used when parsing table rows from text.

.. code-block:: cpp

    ASSERT_EQ(Type::inferFromString("100"), typeInt32);
    ASSERT_EQ(Type::inferFromString("100.00"), typeFloat);
    ASSERT_EQ(Type::inferFromString("192.168.100.1"), typeString);
    ASSERT_EQ(Type::inferFromString("/this/is/a/path"), typeString);


It also contains some other advance functions to work operate on chunks of memory. These methods should
not be commonly used unless a core feature of the library will be developed.

.. code-block:: cpp

    size_t n = 10;
    float * array = new float[n];
    float * array2 = new float[n];
    for (size_t i = 0; i < n; i++)
        array2[i] = i;

    // Copy elements using Type.copy
    typeFloat.copy(array2, array, n);

    // Check correct values
    for (size_t i = 0; i < n; i++)
        ASSERT_EQ(array[i], array2[i]);

    // Test type casting
    int32_t * arrayInt = new int32_t[n];
    typeInt32.operate(Type::CAST, array, typeFloat, arrayInt, n);

    for (size_t i = 0; i < n; ++i)
        ASSERT_FLOAT_EQ(array[i], (float)arrayInt[i]);


Object
------

The :cpp:class:`Object` is implemented as a dynamic single value container. This means that an Object instance can
contains any type and the content change change dynamically during the execution of the program. It relies on the
functionality provided by the :cpp:class:`Type` class.

An Object can be created without value, in which case the default Type is *typeNull*. After that, another value
can be assigned and the type of the object will change if necessary to store the new value. The Object class also
implements some operators that mimic the behaviour of the basic types.

.. code-block:: cpp

    using namespace emcore;

    Object obj;  // Empty object constructor
    ASSERT_EQ(obj.getType(), typeNull);  // The object type should be Null
    ASSERT_TRUE(obj.getType().isNull());

    obj = 1; // Now this object should change its type to typeInt32
    ASSERT_EQ(obj.getType(), typeInt32);

    // After assignment, an object will change its type if necessary
    obj = 2.2f;
    ASSERT_EQ(obj.getType(), typeFloat);

    // Value can be retrieved with get<T>
    ASSERT_FLOAT_EQ(obj.get<float>(), 2.2f);

    // Comparison and other operator are built-in
    ASSERT_EQ(obj, Object(2.2f));

    Object obj2 = 3.2f;
    ASSERT_TRUE(obj2 > obj);
    ASSERT_TRUE(obj < obj2);

    // We can explicitly change its type, internal value will be converted
    obj.setType(typeInt32);
    ASSERT_EQ(obj.get<int>(), (int)2.2f);


Object instances can also contains more complex types such as strings or images:

.. code-block:: cpp

    using namespace emcore;

    const char * str = "This is a test string";
    Object obj3;
    obj3 = std::string(str);
    auto s2 = obj3.get<std::string>();
    ASSERT_EQ(s2, str);  // value is correct

    Image img(ArrayDim(10, 10), typeFloat);
    auto& typeImage = Type::get<Image>();
    obj3 = img;  // Object now contains a pointer to Image
    ASSERT_EQ(obj3.getType(), typeImage);

    Image img2 = obj3.get<Image>();  // Retrieve the image back


The get<T> from :cpp:class:`~emcore::Object` can be used to retrieve the internal value, or to cast it
to another data type:

.. code-block:: cpp

    uint64_t value = 57536865;
    Object o1 = value;
    ASSERT_EQ(o1.getType(), typeUInt64);

    auto value2 = o1.get<uint64_t>();
    ASSERT_EQ(value, value2);

    auto value3 = o1.get<size_t>();
    ASSERT_EQ(value, value3);

    ASSERT_TRUE(o1.get<bool>());


.. _email CCP-EM: ccpem@stfc.ac.uk
.. _pybind11: https://github.com/pybind/pybind11
.. _GPLv3: https://www.gnu.org/licenses/gpl-3.0.en.html