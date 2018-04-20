// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "em/base/error.h"
#include "em/image/image.h"

using namespace em;

int main(int argc, char *argv[]) {
    try {
        if (!ImageIO::hasImpl("tif"))
            THROW_SYS_ERROR(std::string("ImageIO for TIFF does not exist"));
        else
        {
            std::cout << "ImageIO for TIFF exists!!!" << std::endl;
        }


        ImageIO imIO;
        ImageLocation loc;
        std::cout << "ImageIO created" << std::endl;

        std::map<std::string, ArrayDim> fileDims;

        fileDims["/home/joton/projects/image_formats/dm4-empiar/10094/Micrographs_ROI_00/Prefix_3VBSED_roi_00_slice_0000.dm4"] = ArrayDim(3, 3, 1, 1);

        for (auto &pair: fileDims) {
            Image img;
            loc.index = 1;
            loc.path = pair.first;
    //        img.read(loc);
    //        std::cout << "Back in test" << std::endl;
    //        std::cout << img << std::endl;
    //        ArrayDim imgDim(pair.second);
    //        imgDim.n = 1;

            imIO.open(loc.path);
            imIO.toStream(std::cout, 2);

        }
    }
    catch (em::Error &e) {
        std::cout << e << std::endl;
    }

return 0;
}
