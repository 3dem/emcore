emcore
=======
Basic image processing library for Electron Microscopy


Building the library
--------------------

.. code-block:: bash

    git clone git@github.com:3dem/emcore.git
    cd emcore
    mkdir build
    cd build
    # Activating most of development options
    cmake .. -DBUILD_TIFF=ON -DBUILD_PNG=ON -DBUILD_JPEG=ON \
    -DBUILD_TESTS=ON -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON \
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCOPT=ON -DBUILD_PYBIND=ON


Building and installing in a given conda environment
....................................................

.. code-block:: bash

    # First activate the conda environment
    conda activate myEnv
    # Then execute the Python installation script
    python conda/install_to_env.py


Building with Google-tests (REVIEW)
..........................

In order to enable google test, you need to pull the test git-module and pass the proper option to cmake.
From the root emcore directory:

.. code-block:: bash

    cd libs/googletest/
    git submodule update --init

    # Then follow the normal building process...so from build directory:

    cmake .. -DBUILD_TESTS=ON


Generating documentation
.........................

FIXME: Update this section to use Doxygen+Sphinx docs generation


Extra
---------------

Specify path of Conda environment to CMAKE:
...........................................

.. code-block:: bash

    export CMAKE_PREFIX_PATH=/home/josem/installs/devel/miniconda3/envs/py27-emqt/
    # Also remote build artifacts:
    cd build
    rm *
    cmake ...


Specifying a different search root (e.g Kino's machine at LMB)
..............................................................

.. code-block:: bash

    cd build
    rm *
    cmake3 .. -DBUILD_TESTS=ON -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON \
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCOPT=ON -DBUILD_PYBIND=ON \
    -DCMAKE_FIND_ROOT_PATH=./opt/anaconda3/ \
    -DCMAKE_C_COMPILER=/public/gcc/7_2_0/bin/gcc \
    -DCMAKE_CXX_COMPILER=/public/gcc/7_2_0/bin/g++


Building Relion with emcore (old)
............................

For the moment, we need to pass a variable to cmake where the emcore library is:

.. code-block:: bash

    cmake ../relion-devel-tcblab/ -DGUI=OFF -DCUDA=OFF -DALTCPU=OFF \
    -DEMCORE_ROOT_PATH=/home/dari/Projects/emcore/emcore/



