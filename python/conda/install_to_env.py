###!/usr/bin/env bash

import os
import sys


# Define local variables to be used from locals()
prefix = os.environ.get('PREFIX',
                        os.path.dirname(os.path.dirname(sys.executable)))

srcdir = os.environ.get('SRC_DIR', os.getcwd())
build = os.path.join(srcdir, 'build')
python = os.environ.get('PYTHON', 'python')

def system(cmd):
    print(cmd) 
    os.system(cmd)

system('mkdir -p %(build)s && cd %(build)s && rm -rf *' % locals())
os.chdir(build)

opts = ('-DCMAKE_PREFIX_PATH=%(prefix)s '
        '-DCMAKE_INSTALL_PREFIX=%(prefix)s '
        '-DCMAKE_FIND_ROOT_PATH=%(prefix)s ' % locals())

system('cmake .. %s' % opts)
system('make -j 5')
system('make install')

opts = ('-DCMAKE_PREFIX_PATH=%(prefix)s '
        '-DCMAKE_FIND_ROOT_PATH=%(prefix)s ' % locals())
skbuild = os.path.join(srcdir, 'python', '_skbuild')

system('rm -rf %s' % skbuild)
system('cd %(srcdir)s/python; %(python)s setup.py install -- %(opts)s ' % locals())



