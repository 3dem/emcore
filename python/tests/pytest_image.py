
from __future__ import print_function

import os

from base import BaseTest, main
import em


class TestImageLocation(BaseTest):
    def test_basic(self):
        path = '/path/to/image'
        loc2 = em.ImageLocation('/path/to/image')
        self.assertEqual(loc2.index, em.ImageLocation.FIRST)
        self.assertEqual(loc2.path, path)

        loc3 = em.ImageLocation(path, 2)
        self.assertEqual(loc3.path, path)
        self.assertNotEqual(loc2, loc3)
        loc3.index = em.ImageLocation.FIRST
        self.assertEqual(loc2, loc3)


class TestImageIO(BaseTest):

    def test_impl(self):
        for ext in ['mrc', 'mrcs', 'spi', 'spider']:
            self.assertTrue(em.ImageIO.hasImpl(ext))
        spiderIO = em.ImageIO('spi')
        mrcIO = em.ImageIO('mrc')

    def test_readMrc(self):
        mrcIO = em.ImageIO('mrc')

        testDataPath = os.environ.get("EM_TEST_DATA", None)

        if testDataPath:
            # 2D image (a big one): micrograph
            micName = "xmipp_tutorial/micrographs/BPV_1386.mrc"
            micDim = em.ArrayDim(9216, 9441, 1, 1)
            # 2D stack of particles
            stackName = "emx/alignment/Test2/stack2D.mrc"
            stackDim = em.ArrayDim(128, 128, 1, 100)
            # 3D volumes
            vol1Name = "resmap/t20s_proteasome_full.map"
            vol1Dim = em.ArrayDim(300, 300, 300, 1)

            fileDims = {micName: (micDim, 87008256),
                        stackName: (stackDim, 65536), # 128 * 128 * 4
                        vol1Name: (vol1Dim, 108000000)
                        }

            for fn, (dim, size) in fileDims.items():
                img = em.Image()
                loc = em.ImageLocation(os.path.join(testDataPath, fn), 1)
                print(">>> Reading image: ", loc.path)
                img.read(loc)
                dim.n = 1
                self.assertEqual(img.getDim(), dim)
                self.assertEqual(img.getDataSize(), size)


            # Test that we can read also from string
            # that will be converted to ImageLocation
            img = em.Image()
            try:
                img.read(em.ImageLocation(os.path.join(testDataPath, micName)))
                import numpy as np
                a = np.array(img, copy=False)
            except Exception as ex:
                raise ex

            self.assertEqual(img.getDim(), micDim)



class TestImage(BaseTest):
    def test_basic(self):
        img = em.Image()
        self.assertTrue(img.getType().isNull())


if __name__ == '__main__':
    main()


