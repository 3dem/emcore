
import sys
from os.path import dirname, join
import unittest
from unittest import main

# Add the root folder to the path
sys.path.append(join(dirname(dirname(dirname(__file__))), 'build'))


class BaseTest(unittest.TestCase):
    def assertTypeEqual(self, t1, t2):
        # FIXME: Now for some reasons types have different static
        # implementation pointers
        return self.assertEquals(t1.getName(), t2.getName())