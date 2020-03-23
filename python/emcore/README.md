emcore Python module
====================

This folder contains the structure of a Python package.
The binding to some of the C++ classes provided by *emcore* will be generated
with *pybind11*. The .cpp source code can be found in the src folder.

After building of the binding, the _emcore.so file should be generated. When
installing the Python module, this .so file is expected to be inside the
module folder or in the PYTHONPATH, so it can be imported from the __init__.py
file.

The tests folder contains several tests in Python that serve to check that
things are working as expected.

