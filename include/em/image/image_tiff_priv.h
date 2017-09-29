//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_TIFF_H
#define EM_CORE_RW_TIFF_H

#include "em/image/image.h"
#include <tiffio.h>

using namespace em;


struct TiffHeader
{                                   // Header for each Directory in TIFF
    unsigned short  bitsPerSample;
    unsigned short  samplesPerPixel;
    unsigned int   imageWidth;
    unsigned int   imageLength;
    uint16           imageSampleFormat;
    unsigned short  resUnit;
    float            xTiffRes,yTiffRes;
    unsigned int subFileType;
    uint16 pNumber, pTotal; // pagenumber and total number of pages of current directory
    TiffHeader()
    {
        bitsPerSample=samplesPerPixel=0;
        imageWidth=imageLength=subFileType=0;
        imageSampleFormat=0;
        xTiffRes=yTiffRes=0;
    }
}; //TiffHeader


/**
 * Inherit properties from base ImageHandler and add information
 * specific for TIFF format (e.g, the TiffHeader struct)
 */
class ImageTiffHandler: public ImageHandler
{
public:
    TiffHeader header;
    TIFF*      tif;        // TIFF Image file handler

protected:
    /** Open the file for this format. The path and mode
     * should be set before calling this function.
     */
    virtual void openFile();
}; // class ImageTiffHandler


class ImageTiffIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual StringVector getExtensions() const override;

    // TODO: DOCUMENT
    virtual void open(const std::string &path, const FileMode mode=READ_ONLY) override;

    virtual void readImageHeader(const size_t index, Image &image) override {};
    virtual void writeImageHeader(const size_t index, Image &image) override {};

    virtual ~ImageTiffIO();

protected:
    virtual ImageHandler* createHandler() override ;
    virtual void readHeader() override ;
    virtual void writeHeader() override {} ;
    virtual size_t getHeaderSize() const override ;
    virtual ImageIO* create() const override;

}; // class ImageTiffIO

#endif // EM_CORE_RW_TIFF_H