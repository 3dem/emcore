#!/bin/sh

# Remove 'build' content, if compiling from directory
make ${SRC_DIR}/build && cd ${SRC_DIR}/build
echo "Current directory"
pwd
cmake -DCMAKE_PREFIX_PATH=${PREFIX} -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_BUILD_TYPE=Release \
      -DPYTHON_EXECUTABLE=${PYTHON} \
      -DBUILD_EXAMPLES=ON -DBUILD_DOCOPT=ON -DBUILD_TIFF=ON -DBUILD_PNG=ON -DBUILD_JPEG=ON -DBUILD_PYBIND=ON \
      ${SRC_DIR}
make -j 8
make install

mv  ${PREFIX}/lib/emcore.so ${SP_DIR}
