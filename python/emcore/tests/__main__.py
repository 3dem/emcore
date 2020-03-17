
import os
import unittest

testDir = os.path.abspath(os.path.dirname(__file__))

loader = unittest.TestLoader()
tests = loader.discover(testDir)
testRunner = unittest.runner.TextTestRunner()
testRunner.run(tests)
