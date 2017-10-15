//#include <cstdio>

//#ifdef NEVER

#include "tiffio.h"

#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

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
 * Inherit properties from base ImageIOImpl and add information
 * specific for TIFF format (e.g, the TiffHeader struct)
 */
class ImageIOTiff: public ImageIOImpl
{
public:
    std::vector<TiffHeader> vHeader;
    TIFF*      tif;        // TIFF Image file handler

    /** Open the file for this format. The path and mode
     * should be set before calling this function.
     */
    void openFile() override
    {
        // We must check if this warning are necessary
        //TIFFSetWarningHandler(NULL); // Switch off warning messages

        tif = TIFFOpen(path.c_str(), getOpenMode(fileMode));

        if (tif == nullptr)
            THROW_SYS_ERROR(std::string("Error opening file ") + path);

    }

    /** Close the file for this format.
     */
    void closeFile() override
    {
        TIFFClose(tif);

        //TODO: we have to evaluate if we want to check this issue or relay onto programmer
        /* When creating a TIFF file without adding an image the file is 8 bytes
         * and this same file returns an error when trying to open again, we are going
         * to suppose that under 8 bytes this is empty.
        */
        //if (fileName.getFileSize() < 9)
        //    filename.deleteFile();
    }


    void readHeader() override
    {
        char*  tif_buf = NULL;
        TiffHeader dhRef;

        /* Get TIFF image properties */
        do
        {
            dhRef.imageSampleFormat = SAMPLEFORMAT_VOID;
            if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,  &dhRef.bitsPerSample) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_BITSPERSAMPLE");
            if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL,&dhRef.samplesPerPixel) == 0)
                dhRef.samplesPerPixel = 1;

            if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,     &dhRef.imageWidth) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_IMAGEWIDTH");
            if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH,    &dhRef.imageLength) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_IMAGELENGTH");
            if (TIFFGetField(tif, TIFFTAG_SUBFILETYPE,    &dhRef.subFileType) == 0)
                dhRef.subFileType = 0; // Some scanners does not provide this label. So, we set this to zero
            //            REPORT_ERROR(ERR_IO_NOREAD,"rwTIFF: Error reading TIFFTAG_SUBFILETYPE");
            TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT,   &dhRef.imageSampleFormat);
            TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &dhRef.resUnit);
            TIFFGetField(tif, TIFFTAG_XRESOLUTION,    &dhRef.xTiffRes);
            TIFFGetField(tif, TIFFTAG_YRESOLUTION,    &dhRef.yTiffRes);
            TIFFGetField(tif, TIFFTAG_PAGENUMBER,     &dhRef.pNumber, &dhRef.pTotal);

            if ((dhRef.subFileType & 0x00000001) != 0x00000001) //add image if not a thumbnail
                vHeader.push_back(dhRef);
        }
        while(TIFFReadDirectory(tif));

        // TODO: swap management
//    swap = TIFFIsByteSwapped(tif);

        dim.x = vHeader[0].imageWidth;
        dim.y = vHeader[0].imageLength;
        dim.z = 1;
        dim.n = vHeader.size();

        int mode = vHeader[0].bitsPerSample + vHeader[0].imageSampleFormat;
        type = getTypeFromMode(mode);

        // TODO: EMan2 does not write the datatype, using Float by default (do we fix it?)
    }


protected:

    void writeHeader() override
    {
        THROW_ERROR("ImageIOTiff::writeHeader not implemented yet.");
    }

    void readImageData(const size_t index, Image &image) override
    {
        size_t itemSize = getImageSize(); // Size of an item containing the padSize
        size_t padSize = getPadSize();
        size_t readSize = itemSize - padSize;

        char * data;
        data = static_cast<char*> (image.getDataPointer());

        char*  tif_buf = nullptr;
        size_t x, y;

        // Scanline stuff
        uint32 rowsperstrip;
        tsize_t scanline;

        // Dimensions of tiles
        unsigned int tileWidth, tileLength;

        size_t idx = index - 1;
        TIFFSetDirectory(tif,(tdir_t) idx);

        TiffHeader header = vHeader[idx];

        // If samplesPerPixel is higher than 3 it means there are extra samples, as associated alpha data
        // Greyscale images are usually samplesPerPixel=1
        // RGB images are usually samplesPerPixel=3 (this is only implemented for untiled 8-bit tiffs)
        if (header.samplesPerPixel > 3)
            header.samplesPerPixel = 1;

        if (TIFFIsTiled(tif))
        {
            TIFFGetField(tif, TIFFTAG_TILEWIDTH,  &tileWidth);
            TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);
            tif_buf = (char*)_TIFFmalloc(TIFFTileSize(tif));
        }
        else
        {
            TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
            scanline = TIFFScanlineSize(tif);
            tif_buf = (char*)_TIFFmalloc(scanline);
        }
        if (tif_buf == 0)
        {
            TIFFError(TIFFFileName(tif), "No space for strip buffer");
            THROW_ERROR("ImageIOTiff: strip buffer allocation failed.");
        }

        /* Start to convert the TIFF image to type T */

        int mode = header.bitsPerSample + header.imageSampleFormat;

        if (TIFFIsTiled(tif))
        {
            THROW_ERROR("ImageIOTiff::readData not implemented yet for Tiled files.");

            /*           for (y = 0; y < vHeader[0].imageLength; y += tileLength)
                           for (x = 0; x < vHeader[0].imageWidth; x += tileWidth)
                           {
                               TIFFReadTile(tif, tif_buf, x, y, 0, 0);
                               if (swap)
                                   swapPage((char*)tif_buf, TIFFTileSize(tif)*sizeof(unsigned char), datatype);

                               castTiffTile2T((pad*imReaded), tif_buf, x, y,
                                              header.imageWidth, header.imageLength,
                                              tileWidth, tileLength,
                                              header.samplesPerPixel,
                                              datatype);
                           }
           */        }
        else
        {
            size_t readSize = header.imageWidth * type->getSize();

            for (y = 0; y < header.imageLength; y++)
            {
                TIFFReadScanline(tif, tif_buf, y);
                memcpy(data + y*readSize, tif_buf, readSize);
            }
        }

        _TIFFfree(tif_buf);
//        THROW_ERROR("ImageIOTiff::readImageData not implemented yet.");
    }


    void writeImageData(const size_t index, const Image &image) override
    {
        THROW_ERROR("ImageIOTiff::writeImageData not implemented yet.");
    }

    size_t getHeaderSize() const override
    {
        return 0;
    }


    const TypeMap &getTypeMap() const override
    {
        static const TypeMap tm = {{8+SAMPLEFORMAT_UINT, TypeUInt8},
                                   {8+SAMPLEFORMAT_INT,  TypeInt8},
                                   {16+SAMPLEFORMAT_UINT, TypeUInt16},
                                   {16+SAMPLEFORMAT_INT, TypeInt16},
                                   {32+SAMPLEFORMAT_UINT, TypeUInt32},
                                   {32+SAMPLEFORMAT_INT, TypeInt32},
                                   {32+SAMPLEFORMAT_IEEEFP, TypeFloat}};
        return tm;
    }


protected:

}; // class ImageIOTiff

StringVector tiffExts = {"tif", "tiff"};

REGISTER_IMAGE_IO(tiffExts, ImageIOTiff);

//#endif