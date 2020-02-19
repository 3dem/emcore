Docker Notes to build PyPI wheels
=================================

Following tutorial from here:
https://python-packaging-tutorial.readthedocs.io/en/latest/binaries_dependencies.html

Python packaging docs:
https://packaging.python.org/guides/distributing-packages-using-setuptools/#wheels

Nice thread about building manylinux with recent C++
https://github.com/pypa/manylinux/issues/118

Using docker image:
dockcross/manylinux2010-x86

Nice docker tips:
https://spin.atomicobject.com/2018/10/04/docker-command-line/


Building a Docker Image based on dockcross/manylinux2010-x86
------------------------------------------------------------

.. code-block:: bash

    docker build -t manylinux2010-emcore docker/

This will use the Dockerfile under the docker folder:

.. code-block::

    FROM dockcross/manylinux2010-x64

    LABEL description="manylinux image from drockcross to build emcore wheels"
    LABEL maintainer="J.M. de la Rosa Trevin delarosatrevin@gmail.com"

    RUN yum install -y libtiff-devel libpng-devel libjpeg-devel fftw3-devel sqlite-devel


Running the image to build the wheels
-------------------------------------

.. code-block:: bash

    # Run the docker image mounting the emcore folder as /work in the container
    docker run --rm -it -v $PWD/emcore:/work manylinux2014-emcore bash

    # Testing cmake directly under the build folder
    cmake .. -DCMAKE_FIND_ROOT_PATH=/opt/_internal/cpython-3.5.9/

    # Or just building the wheel:
    /opt/_internal/cpython-3.5.9/bin/pip wheel -w dist .

    # Or building all wheels
    for cp in /opt/_internal/cpython-3.[5-8]*; do $cp/bin/pip wheel -w dist .; done

