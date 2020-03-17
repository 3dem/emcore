import os
import sys
from glob import glob

from skbuild import setup

here = os.path.abspath(os.path.dirname(__file__))

version = None
with open(os.path.join(os.path.dirname(here), 'VERSION.txt')) as f:
    version = f.readline()

setup(
    name="emcore",
    version=version,
    description="Python module to access functionality in the emcore library",
    long_description="",
    url="https://github.com/3dem/emcore",
    author='J.M. De la Rosa Trevin',
    author_email='delarosatrevin@scilifelab.se',  # Optional
    keywords='electron-microscopy cryo-em structural-biology image-processing',
    license="GPLv3",
    packages=['emcore',
              'emcore.tests'],
    test_suite='emcore.tests'
)
