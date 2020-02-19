
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
        vol1Name = "resmap/t20s_proteasome_full.map"
        vol1Dim = emc.ArrayDim(300, 300, 300, 1)

        n = 5
        m = 5
        img = emc.Image()
        try:
            loc = emc.ImageLocation(os.path.join(dataPath, vol1Name), 1)
            img.read(loc)
            a = np.array(img, copy=False)
            dim = img.getDim()

            # show central slice on each axis
            aa = np.zeros((dim.y, dim.x * 3), dtype=a.dtype)

            sliceZ = dim.z / 2
            aa[:, 0:dim.x] = a[sliceZ, :, :]

            sliceY = dim.y / 2
            aa[:, dim.x:2*dim.x] = a[:, sliceY, :]

            sliceX = dim.x / 2
            aa[:, 2*dim.x:3*dim.x] = a[:, :, sliceX]

            print("Showing volume slices in matplotlib...")
            imgplot = plt.imshow(aa, cmap=plt.cm.gray)
            plt.show()

        except Exception as ex:
            print("Exception: ", str(ex))


