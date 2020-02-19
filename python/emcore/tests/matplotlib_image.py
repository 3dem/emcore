
from __future__ import print_function
import os
import matplotlib.pyplot as plt
import numpy as np

from base import BaseTest # only used to put em in the path now
import emcore as emc


if __name__ == '__main__':
    mrcIO = emc.ImageFile('mrc')

    dataPath = os.environ.get("EM_TEST_DATA", None)

    if dataPath:
        micName = "xmipp_tutorial/micrographs/BPV_1386.mrc"
        micDim = emc.ArrayDim(9216, 9441, 1, 1)
        stackName = "emx/alignment/Test2/stack2D.mrc"
        stackDim = emc.ArrayDim(128, 128, 1, 100)
        fileDims = {micName: micDim, stackName: stackDim}

        n = 5
        m = 5
        img = emc.Image()
        try:
            loc = emc.ImageLocation(os.path.join(dataPath, stackName))

            # Read many images from the particles stack and put them
            # together using a numpy array and showing in matplotlib
            for i in range(n * m):
                loc.index = i + 1
                img.read(loc)
                a = np.array(img, copy=False)
                dim = img.getDim()
                iy = i / m
                ix = i % m
                print("dim: ", dim, "type: ", img.getType())
                print("i: ", i, "iy: ", iy, "ix: ", ix)

                if i == 0: # only first time
                    aa = np.zeros((dim.y * n, dim.x * m), dtype=a.dtype)
                    print("Created big array, shape", aa.shape)

                aa[iy*dim.y:(iy+1)*dim.y, ix*dim.x:(ix+1)*dim.x] = a

            print("Showing image in matplotlib...")
            imgplot = plt.imshow(aa, cmap=plt.cm.gray)
            plt.show()

        except Exception as ex:
            print("Exception: ", str(ex))


