
import sys
from datetime import datetime
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


def prettyDelta(timedelta):
    """ Remove the milliseconds of the timedelta. """
    return str(timedelta).split('.')[0]


class Timer():
    """ Simple Timer base in datetime.now and timedelta. """
    def tic(self):
        self._dt = datetime.now()

    def getToc(self):
        return prettyDelta(datetime.now()-self._dt)

    def toc(self, message='Elapsed:'):
        print(message, self.getToc())