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
