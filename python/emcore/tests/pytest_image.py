
from __future__ import print_function

import os

from base import BaseTest, main
import emcore as emc


class TestImageLocation(BaseTest):
    def test_basic(self):
        path = '/path/to/image'
        loc2 = emc.ImageLocation('/path/to/image')
        self.assertEqual(loc2.index, emc.ImageLocation.FIRST)
        self.assertEqual(loc2.path, path)

        loc3 = emc.ImageLocation(path, 2)
        self.assertEqual(loc3.path, path)
        self.assertNotEqual(loc2, loc3)
        loc3.index = emc.ImageLocation.FIRST
        self.assertEqual(loc2, loc3)


class TestImageFile(BaseTest):

    def test_static(self):
        for ext in ['mrc', 'mrcs', 'spi', 'spider']:
            self.assertTrue(emc.ImageFile.hasImpl(ext))
        formatTypes = emc.ImageFile.getFormatTypes()
        self.assertEqual(emc.ImageFile.getImplTypes("spider"), formatTypes["spider"])
        self.assertEqual(emc.ImageFile.getImplTypes("mrc"), formatTypes["mrc"])
        self.assertEqual(emc.ImageFile.getImplTypes("img"), formatTypes["imagic"])

    def test_readMrc(self):
        mrcIO = emc.ImageFile()

        testDataPath = os.environ.get("EM_TEST_DATA", None)

        if testDataPath:
            # 2D image (a big one): micrograph
            micName = "xmipp_tutorial/micrographs/BPV_1386.mrc"
            micDim = emc.ArrayDim(9216, 9441, 1, 1)
            # 2D stack of particles
            stackName = "emx/alignment/Test2/stack2D.mrc"
            stackDim = emc.ArrayDim(128, 128, 1, 100)
            # 3D volumes
            vol1Name = "resmap/t20s_proteasome_full.map"
            vol1Dim = emc.ArrayDim(300, 300, 300, 1)

            fileDims = {micName: (micDim, 87008256),
                        stackName: (stackDim, 65536), # 128 * 128 * 4
                        vol1Name: (vol1Dim, 108000000)
                        }

            for fn, (dim, size) in fileDims.items():
                img = emc.Image()
                loc = emc.ImageLocation(os.path.join(testDataPath, fn), 1)
                print(">>> Reading image: ", loc.path)
                img.read(loc)
                dim.n = 1
                self.assertEqual(img.getDim(), dim)
                self.assertEqual(img.getDataSize(), size)


            # Test that we can read also from string
            # that will be converted to ImageLocation
            img = emc.Image()
            try:
                img.read(emc.ImageLocation(os.path.join(testDataPath, micName)))
                import numpy as np
                a = np.array(img, copy=False)
            except Exception as ex:
                raise ex

            self.assertEqual(img.getDim(), micDim)


class TestImage(BaseTest):
    def test_basic(self):
        img = emc.Image()
        self.assertTrue(img.getType().isNull())


if __name__ == '__main__':
    main()


