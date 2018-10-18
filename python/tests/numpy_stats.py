#!/usr/bin/env python
from __future__ import print_function

import em
import numpy as np
import sys

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Provide input file(s). ")
        sys.exit(1)

    img = em.Image()

    print("Computing stats with numpy: ")

    for fn in sys.argv[1:]:
        loc = em.ImageLocation(fn)
        img.read(loc)
        data = np.array(img, copy=False)
        print("\nFile: ", fn,
              "\nmin:", np.amin(data), "max:", np.amax(data),
              "mean: %0.5f" % np.mean(data), "std: %0.5f" % np.std(data))

