// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "em/base/error.h"
#include "em/image/image.h"

using namespace em;

int main(int argc, char *argv[]) {
    try {
        if (!ImageIO::has("tif"))
            THROW_SYS_ERROR(std::string("ImageIO for TIFF does not exist"));
        else
        {
            std::cout << "ImageIO for TIFF exists!!!" << std::endl;
        }


        ImageIO *imIO = ImageIO::get("tif");
        ImageLocation loc;
        std::cout << "ImageIO created" << std::endl;

        std::map<std::string, ArrayDim> fileDims;

        fileDims["/home/joton/projects/em-core/resources/images/singleImage.tif"] = ArrayDim(3, 3, 1,
                                                                 1);

        for (auto &pair: fileDims) {
            Image img;
            loc.index = 1;
            loc.path = pair.first;

            imIO->read(loc, img);
            std::cout << "Back in test" << std::endl;
            std::cout << img << std::endl;
            ArrayDim imgDim(pair.second);
            imgDim.n = 1;
        }
        delete imIO;

    }
    catch (em::Error &e) {
        std::cout << e << std::endl;
    }

return 0;
}
