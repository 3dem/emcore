// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "em/base/error.h"
#include "em/image/image.h"

using namespace em;

int main (int argc, char *argv[])
{
    ImageIO * spiIO = ImageIO::get("spi");
    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;
    auto testDataPath = getenv("EM_TEST_DATA");
    std::string root(testDataPath);

    fileDims["emx/alignment/testAngles/proj.spi"] = ArrayDim(128, 128, 1, 1);

    for (auto &pair: fileDims)
    {
        Image img;
        loc.index = 1;
        loc.path = root + pair.first;
        spiIO->read(loc, img);
        std::cout << "Back in test" << std::endl;
        std::cout << img << std::endl;
        ArrayDim imgDim(pair.second);
        imgDim.n = 1;
    }

    delete spiIO;

    /*if (argc < 2)
      {
      fprintf(stdout,"Usage: %s number\n",argv[0]);
      return 1;
      }
    double inputValue = atof(argv[1]);
    double outputValue = sqrt(inputValue);
    fprintf(stdout,"The square root of %g is %g\n",
            inputValue, outputValue);*/
  return 0;
}
