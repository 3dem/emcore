###!/usr/bin/env bash

import os
import sys
import site

pathDict = {
    'sitePackages': site.getsitepackages()[0],
    'prefix': os.path.dirname(os.path.dirname(sys.executable))
}

def system(cmd):
    print(cmd) 
    os.system(cmd)

system('mkdir -p build && cd build && rm -rf *')
os.chdir('build')
cmakeConfigOpts = ('-DCMAKE_PREFIX_PATH=%(prefix)s '
                   '-DCMAKE_INSTALL_PREFIX=%(prefix)s '
                   '-DCMAKE_FIND_ROOT_PATH=%(prefix)s ' % pathDict)

system('cmake .. %s' % cmakeConfigOpts)
system('make -j 5')
system('make install')

cmakeConfigOpts = ('-DCMAKE_PREFIX_PATH=%(prefix)s '
                   '-DCMAKE_FIND_ROOT_PATH=%(prefix)s ' % pathDict)
system('cd ../python; python setup.py install -- %s' % cmakeConfigOpts)
#system('mv %(prefix)s/lib/emcore.so %(sitePackages)s/' % pathDict)


