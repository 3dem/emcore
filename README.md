# em-core
Basic image processing library for Electron Microscopy

## Building the library

```
git clone git@github.com:3dem/em-core.git
cd em-core
mkdir build
cd build
cmake ..
```

### Building with Google-tests
In order to enable google test, you need to pull the test git-module and pass the proper option to cmake.
From the root em-core directory:
```
cd libs/googletest/
git submodule update --init

# Then follow the normal building process...so from build directory:

cmake .. -DBUILD_TESTS=ON 
```
### Building with Docs generation
To build the documentation you could add the following option to cmake:
```
cmake .. -DBUILD_DOCS=ON 
```
### Common development cmake options:
While developing the library, we usually want also to build the tests and use cmake Debug mode:
```
cmake .. -DBUILD_TESTS=ON -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
```

## Building Relion with em-core

For the moment, we need to pass a variable to cmake where the em-core library is:
```
cmake ../relion-devel-tcblab/ -DGUI=OFF -DCUDA=OFF -DALTCPU=OFF \
-DEMCORE_ROOT_PATH=/home/dari/Projects/emcore/em-core/
```

### Specifying a different Python version (REVIEW)
If you have more than one Python installed in your system, maybe it is useful to specify which one you want to use. 
In the following example it will point to the Python installed via Anaconda.

```
cmake .. -DEMCORE_ROOT_PATH=$HOME/work/development/em-core \
-DPYTHON_LIBRARIES=~/installs/anaconda2/lib/libpython2.7.so \
-DPYTHON_EXECUTABLE=~/installs/anaconda2/bin/python2.7
```
## Troubleshooting

### Specify path of Conda environment to CMAKE:
```
export CMAKE_PREFIX_PATH=/home/josem/installs/devel/miniconda3/envs/py27-emqt/
# Also remote build artifacts:
cd build
rm *
cmake ...
```
### Specifying a different search root (e.g Kino's machine at LMB)
```
cd build
rm *
cmake3 .. -DBUILD_TESTS=ON -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCOPT=ON -DBUILD_PYBIND=ON \
  -DCMAKE_FIND_ROOT_PATH=~/opt/anaconda3/ \
  -DCMAKE_C_COMPILER=/public/gcc/7_2_0/bin/gcc \
  -DCMAKE_CXX_COMPILER=/public/gcc/7_2_0/bin/g++
```


