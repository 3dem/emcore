#include <cstdio>

#include "em/image/image_tiff_priv.h"


std::string ImageTiffIO::getName() const
{
    return "tiff";
}

StringVector ImageTiffIO::getExtensions() const
{
    return {"tiff"};
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

em::ImageIO* ImageTiffIO::create() const
{
    return new ImageTiffIO();
}
