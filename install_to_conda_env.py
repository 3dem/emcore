###!/usr/bin/env bash

import os, sys
import site

sitePackages = site.getsitepackages()[0]
prefix = os.path.dirname(os.path.dirname(sys.executable))

def system(cmd):
    print(cmd) 
    os.system(cmd)

system('cd build; rm -rf *')
os.chdir('build')
system('cmake .. -DCMAKE_PREFIX_PATH=%(prefix)s -DCMAKE_INSTALL_PREFIX=%(prefix)s '
       '-DBUILD_TIFF=ON -DBUILD_PNG=ON -DBUILD_JPEG=ON '
       '-DBUILD_TESTS=ON -DBUILD_DOCS=ON -DBUILD_EXAMPLES=ON '
       '-DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCOPT=ON -DBUILD_PYBIND=ON '
       '-DCMAKE_FIND_ROOT_PATH=%(prefix)s' % locals())
system('make -j 5')
system('make install')
system('mv %(prefix)s/lib/em.so %(sitePackages)s/' % locals())

