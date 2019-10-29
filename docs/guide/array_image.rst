Working with Array and Image
============================

Array
-----

The :cpp:class:`~emcore::Array` class is a four-dimensional container of values. This means, that it can
holds N elements where each element is usually a 2D or 3D array of values. (i.g images or volumes).
Similar to :cpp:class:`~emcore::Object`, the Array can have a dynamic type that is the same of all values.
The Array class has been designed and implemented to make an efficient use of the memory, avoiding unncessary
operations. For example, if an array is resized, but the total ammount of memory is enough, it will be
reused, instead of trying to allocate new memory.

The dimensions of the Array are stored in a simple class :cpp:class:`~emcore::ArrayDim`, that essentially
have just *x*, *y*, *z* and *n* attributes. the ArrayDim class has a few more utility functions to compute
the total size of given chunks of the described array.







Image
-----

