
import sys
from os.path import dirname, join
import unittest
from unittest import main

# Add the root folder to the path
sys.path.append(join(dirname(dirname(dirname(__file__))), 'build'))


class BaseTest(unittest.TestCase):
    pass