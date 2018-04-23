# em-py
Python binding to em-core library base on Pybind11 to export C++ classes to Python 

## Building the library

```
git clone git@github.com:3dem/em-py.git
cd em-py
mkdir build
cd build
cmake .. -DEMCORE_ROOT_PATH=$HOME/work/development/em-core
```

Basically we need to pass the EMCORE_ROOT_PATH to link against. 
This will be changed when we improve the cmake file to discover where em-core is.

### Specifying a different Python version
If you have more than one Python installed in your system, maybe it is useful to specify which one you want to use. 
In the following example it will point to the Python installed via Anaconda.

```
cmake .. -DEMCORE_ROOT_PATH=$HOME/work/development/em-core \
-DPYTHON_LIBRARIES=~/installs/anaconda2/lib/libpython2.7.so \
-DPYTHON_EXECUTABLE=~/installs/anaconda2/bin/python2.7
```

