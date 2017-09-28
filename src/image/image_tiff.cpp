#include <cstdio>

#include "em/image/image_priv.h"
#include "em/image/image_tiff_priv.h"



std::string ImageTiffIO::getName() const
{
    return "tiff";
}

StringVector ImageTiffIO::getExtensions() const
{
    return {"tiff"};
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

size_t ImageTiffIO::getHeaderSize() const
{
    return 0; // FIXME
}

size_t ImageTiffIO::getPadSize() const
{
    return 0;
}

em::ImageIO* ImageTiffIO::create() const
{
    return new ImageTiffIO();
}

REGISTER_IMAGE_IO(ImageTiffIO);