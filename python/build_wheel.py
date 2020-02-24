#!/usr/bin/env python
"""
Build the emcore wheel for one Python interpreter or for
all of 3.x versions in the manylinux2010 docker image.
"""

import os
import sys
from glob import glob

here = os.path.abspath(os.path.dirname(__file__))
version = None
with open(os.path.join(os.path.dirname(here), 'VERSION.txt')) as f:
    version = f.readline()

usage = """
Usage:
    build_wheel.py [all | path_to_python_interpreter] [--dont_emcore]
"""

cpython_prefix = '/opt/_internal/cpython-3'


def system(cmd):
    print(cmd) 
    os.system(cmd)


def build_wheels(interp_path_list, build_emcore=True):
    """
    Build different wheels for each interpreter in the list.
    """
    if not interp_path_list:
        raise Exception("Empty list of interpreters. ")

    os.chdir(os.path.dirname(here))  # em-core root directory

    if build_emcore:
        system("mkdir -p build && cd build && rm -rf *")
        system("cd build && cmake .. && make -j 5 && make install")

    os.chdir('python')
    for interp_path in interp_path_list:
        parts = interp_path.strip().split('.')
        print(parts)

        if parts[0] != cpython_prefix:
            print("Error with input %s" % interp_path)
            print("Expecting to start with: %s" % cpython_prefix)

        system('%s/bin/python setup.py bdist_wheel && rm -rf _skbuild'
               % interp_path)

        args = {
            'py': '3' + parts[1],
            'v': version,
            'm': 'm' if parts[1] != '8' else ''
        }
        wheelname = 'emcore-%(v)s-cp%(py)s-cp%(py)s%(m)s-linux_x86_64.whl' % args
        system('auditwheel repair dist/%s -w dist/wheelhouse' % wheelname)


if len(sys.argv) < 2:
    print("Please provide input argument (all or python interpreter root path")
    print(usage)
    sys.exit(1)

arg = sys.argv[1]
build_emcore = '--dont_emcore' not in sys.argv

interp_list = [arg] if arg != 'all' else glob(cpython_prefix + '*')
build_wheels(interp_list, build_emcore=build_emcore)





