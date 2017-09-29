#include <cstdio>

#include "em/base/error.h"
#include "em/image/image_priv.h"
#include "em/image/image_tiff_priv.h"






/** Open the file for this format. The path and mode
 * should be set before calling this function.
 */
void ImageTiffHandler::openFile()
{
    //TIFFSetWarningHandler(NULL); // Switch off warning messages
    tif = TIFFOpen(path.c_str(), getOpenMode(fileMode));

    if (tif == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file '") + path);
    
}

ImageHandler* ImageTiffIO::createHandler()
{
    return new ImageTiffHandler;
} // createHandler

// TODO: DOCUMENT
void ImageTiffIO::open(const std::string &path, const FileMode mode)
{
    auto tiffHandler = static_cast<ImageTiffHandler*>(handler);


}

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

REGISTER_IMAGE_IO(ImageTiffIO);