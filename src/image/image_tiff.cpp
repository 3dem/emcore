#include <cstdio>

#include "em/image/image_tiff_priv.h"


std::string ImageTiffIO::getName() const
{
    return "tiff";
}

std::string ImageTiffIO::getExtensions() const
{
    return "tiff";
}


void ImageTiffIO::read(const size_t index, em::Image &image)
{

}


ImageTiffIO::~ImageTiffIO()
{

}

void ImageTiffIO::readHeader()
{
    // TODO: Implement it
}

em::ImageIO* ImageTiffIO::create() const
{
    return new ImageTiffIO();
}
