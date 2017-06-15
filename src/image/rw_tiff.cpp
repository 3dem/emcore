#include <cstdio>

#include "em/image/rw_tiff.h"


std::string TiffIO::getName() const
{
    return "tiff";
}

std::string TiffIO::getExtensions() const
{
    return "tiff";
}

void TiffIO::read(const em::ImageLocation &location, em::Image &image)
{

}

void TiffIO::openFile(const std::string &path)
{

    FILE* imageFile = fopen(path.c_str(), "rb");
    data["imageFile"] = Object(imageFile);
}

void TiffIO::read(const size_t index, em::Image &image)
{

}

void TiffIO::closeFile()
{
    FILE* imageFile = static_cast<FILE*>(data["imageFile"]);
    fclose(imageFile);
}

TiffIO::~TiffIO()
{

}

em::ImageIO* TiffIO::create() const
{
    return new TiffIO();
}
