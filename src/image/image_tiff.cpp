#include <cstdio>

#include "em/base/error.h"
#include "em/image/image_priv.h"
#include "em/image/image_tiff_priv.h"


/** Open the file for this format. The path and mode
 * should be set before calling this function.
 */
void ImageTiffHandler::openFile()
{
    // We must check if this warning are necessary
    //TIFFSetWarningHandler(NULL); // Switch off warning messages

    tif = TIFFOpen(path.c_str(), getOpenMode(fileMode));

    if (tif == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file ") + path);

}

ImageHandler* ImageTiffIO::createHandler()
{
    return new ImageTiffHandler;
} // createHandler


std::string ImageTiffIO::getName() const
{
    return "tiff";
}

StringVector ImageTiffIO::getExtensions() const
{
    return {"tif", "tiff"};
}

void ImageTiffIO::read(const size_t index, Image &image)
{
    THROW_ERROR("ImageTiffIO::read not implemented yet.");
}

ImageTiffIO::~ImageTiffIO()
{

}


void ImageTiffIO::readHeader()
{
    auto tiffHandler = static_cast<ImageTiffHandler*>(handler);

    char*  tif_buf = NULL;
    auto tif = tiffHandler->tif;
    auto &dirHead = tiffHandler->vHeader;
    TiffHeader dhRef;


    /* Get TIFF image properties */
    do
    {
        dhRef.imageSampleFormat = SAMPLEFORMAT_VOID;
        if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,  &dhRef.bitsPerSample) == 0)
            THROW_SYS_ERROR("ImageTiffIO: Error reading TIFFTAG_BITSPERSAMPLE");
        if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL,&dhRef.samplesPerPixel) == 0)
            dhRef.samplesPerPixel = 1;

        if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,     &dhRef.imageWidth) == 0)
            THROW_SYS_ERROR("ImageTiffIO: Error reading TIFFTAG_IMAGEWIDTH");
        if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH,    &dhRef.imageLength) == 0)
            THROW_SYS_ERROR("ImageTiffIO: Error reading TIFFTAG_IMAGELENGTH");
        if (TIFFGetField(tif, TIFFTAG_SUBFILETYPE,    &dhRef.subFileType) == 0)
            dhRef.subFileType = 0; // Some scanners does not provide this label. So, we set this to zero
        //            REPORT_ERROR(ERR_IO_NOREAD,"rwTIFF: Error reading TIFFTAG_SUBFILETYPE");
        TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT,   &dhRef.imageSampleFormat);
        TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &dhRef.resUnit);
        TIFFGetField(tif, TIFFTAG_XRESOLUTION,    &dhRef.xTiffRes);
        TIFFGetField(tif, TIFFTAG_YRESOLUTION,    &dhRef.yTiffRes);
        TIFFGetField(tif, TIFFTAG_PAGENUMBER,     &dhRef.pNumber, &dhRef.pTotal);

        if ((dhRef.subFileType & 0x00000001) != 0x00000001) //add image if not a thumbnail
            dirHead.push_back(dhRef);
    }
    while(TIFFReadDirectory(tif));

    // TODO: swap management
//    swap = TIFFIsByteSwapped(tif);

    tiffHandler->dim.x = dirHead[0].imageWidth;
    tiffHandler->dim.y = dirHead[0].imageLength;
    tiffHandler->dim.z = 1;
    tiffHandler->dim.n = dirHead.size();

    //TODO: type selection implementation
    tiffHandler->type = em::TypeFloat;
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