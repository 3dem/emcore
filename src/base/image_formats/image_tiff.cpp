//#include <cstdio>

//#ifdef NEVER

#include "tiffio.h"

#include "em/base/error.h"
#include "em/base/image.h"
#include "em/base/image_priv.h"

using namespace em;


struct TiffHeader
{                                   // Header for each Directory in TIFF
    uint16          dirIndex;      // Directory index
    unsigned short  bitsPerSample;
    unsigned short  samplesPerPixel;
    unsigned int    imageWidth;
    unsigned int    imageLength;
    uint16          imageSampleFormat;
    unsigned short  resUnit;
    float           xTiffRes,yTiffRes;
    unsigned int    subFileType;
    uint16          pNumber, pTotal; // pagenumber and total number of pages of current directory
    TiffHeader()
    {
        bitsPerSample=samplesPerPixel=resUnit=0;
        imageWidth=imageLength=subFileType=0;
        imageSampleFormat=pNumber=pTotal=0;
        xTiffRes=yTiffRes=0;
    }
}; //TiffHeader


/**
 * Inherit properties from base ImageFile::Impl and add information
 * specific for TIFF format (e.g, the TiffHeader struct)
 */
class ImageIOTiff: public ImageFile::Impl
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

        tif = TIFFOpen(path.c_str(), getModeString());

        if (tif == nullptr)
            THROW_SYS_ERROR(std::string("Error opening file ") + path);
    } // function openFile

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
        //TODO update: we always write allocate an image in TIFF file directory
        // so never there will never be this problem anymore
        //if (fileName.getFileSize() < 9)
        //    filename.deleteFile();
    }


    void readHeader() override
    {
        TiffHeader header;

        /* Get TIFF image properties */
        do
        {
            header.imageSampleFormat = SAMPLEFORMAT_VOID;
            if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,  &header.bitsPerSample) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_BITSPERSAMPLE");
            if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL,&header.samplesPerPixel) == 0)
                header.samplesPerPixel = 1;

            if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,     &header.imageWidth) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_IMAGEWIDTH");
            if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH,    &header.imageLength) == 0)
                THROW_SYS_ERROR("ImageIOTiff: Error reading TIFFTAG_IMAGELENGTH");
            if (TIFFGetField(tif, TIFFTAG_SUBFILETYPE,    &header.subFileType) == 0)
                header.subFileType = 0; // Some scanners does not provide this label. So, we set this to zero
            //            REPORT_ERROR(ERR_IO_NOREAD,"rwTIFF: Error reading TIFFTAG_SUBFILETYPE");
            TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT,   &header.imageSampleFormat);
            TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &header.resUnit);
            TIFFGetField(tif, TIFFTAG_XRESOLUTION,    &header.xTiffRes);
            TIFFGetField(tif, TIFFTAG_YRESOLUTION,    &header.yTiffRes);
            TIFFGetField(tif, TIFFTAG_PAGENUMBER,     &header.pNumber, &header.pTotal);

            if ((header.subFileType & 0x00000001) != 0x00000001) //add image if not a thumbnail
            {
                header.dirIndex = TIFFCurrentDirectory(tif);
                vHeader.push_back(header);
            }
        }
        while(TIFFReadDirectory(tif));

        // TODO: swap management
//    swap = TIFFIsByteSwapped(tif);

        dim.x = vHeader[0].imageWidth;
        dim.y = vHeader[0].imageLength;
        dim.z = 1;
        dim.n = vHeader.size();
        // We obtain single value mode by adding bitspersample and sampleformat
        int mode = vHeader[0].bitsPerSample + vHeader[0].imageSampleFormat;
        type = getTypeFromMode(mode);
        // TODO: EMan2 does not write the datatype, using Float by default (do we fix it?)
    }

    void writeHeader() override
    {
        vHeader.clear();

        TiffHeader header;

        header.imageWidth = (unsigned int) dim.x;
        header.imageLength = (unsigned int) dim.y;

        header.samplesPerPixel = 1; //One channel images
        header.resUnit = RESUNIT_CENTIMETER;

        //Setting bitsPerSample and imageSampleFormat;
        setHeaderType(header);

        header.xTiffRes = 1;
        header.yTiffRes = 1;

        //Write each image in a directory
        for (size_t i = 0; i < dim.n; ++i )
        {
            vHeader.push_back(header);

            TIFFSetDirectory(tif,(tdir_t) i);

            // Image header
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL,header.samplesPerPixel);
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,  header.bitsPerSample);
            TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT,   header.imageSampleFormat);
            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,     header.imageWidth);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH,    header.imageLength);
            TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, header.resUnit);
            TIFFSetField(tif, TIFFTAG_XRESOLUTION,    header.xTiffRes);
            TIFFSetField(tif, TIFFTAG_YRESOLUTION,    header.yTiffRes);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,    PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif, TIFFTAG_COMPRESSION,    COMPRESSION_NONE);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,   (uint32) header.imageLength);
            TIFFSetField(tif, TIFFTAG_PLANARCONFIG,   PLANARCONFIG_CONTIG);
            TIFFSetField(tif, TIFFTAG_ORIENTATION ,   ORIENTATION_TOPLEFT);
            TIFFSetField(tif, TIFFTAG_SOFTWARE,       EM_CORE_VERSION);

            //if (dim.n == 1 && isStack == false)
            if (dim.n == 1)
            {
                TIFFSetField(tif, TIFFTAG_SUBFILETYPE, (unsigned int) 0x0);
                TIFFSetField(tif, TIFFTAG_PAGENUMBER, (uint16) 0, (uint16) 0);
            }
            else
            {
                TIFFSetField(tif, TIFFTAG_SUBFILETYPE, (unsigned int) 0x2);
                TIFFSetField(tif, TIFFTAG_PAGENUMBER, (uint16) i, (uint16) dim.n);
            }

            size_t writeBuffer = header.imageWidth * type.getSize();

            char*  tif_buf = nullptr;
            tif_buf = (char*)_TIFFmalloc(writeBuffer);

            // We create the image to allocate the space in directory to avoid
            //   because we don't know how to add the image to directory a posteriori
            for (size_t y = 0; y < header.imageLength; ++y)
                TIFFWriteScanline(tif, tif_buf, y, 0);

            TIFFWriteDirectory(tif);
        }
    }

    void readImageData(const size_t index, Image &image) override
    {
        char * data;
        data = static_cast<char*> (image.getData());

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

        // If samplesPerPixel is higher than 3 it means there are extra samples,
        // as associated alpha data
        // Greyscale images are usually samplesPerPixel=1
        // RGB images are usually samplesPerPixel=3 (this is only implemented
        // for untiled 8-bit tiffs)
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
            size_t readSize = header.imageWidth * type.getSize();

            for (y = 0; y < header.imageLength; y++)
            {
                TIFFReadScanline(tif, tif_buf, y);
                memcpy(data + y*readSize, tif_buf, readSize);
            }
        }
        _TIFFfree(tif_buf);
    }

    void writeImageData(const size_t index, const Image &image) override
    {
        size_t idx = index - 1;
        TiffHeader &header = vHeader[idx];

        TIFFSetDirectory(tif,(tdir_t) idx);

        char*  tif_buf = nullptr;
        size_t writeBuffer = header.imageWidth * type.getSize();
        tif_buf = (char*)_TIFFmalloc(writeBuffer);

        if (tif_buf == 0)
        {
            TIFFError(TIFFFileName(tif), "No space for strip buffer");
            THROW_ERROR("ImageIOTiff: strip buffer allocation failed.");
        }

        char * data;
        data = const_cast<char*>(static_cast<const char*> (image.getData()));

        for (size_t y = 0; y < header.imageLength; ++y)
        {
            memcpy(tif_buf, data + y*writeBuffer, writeBuffer);
            TIFFWriteScanline(tif, tif_buf, y, 0);
        }

        TIFFWriteDirectory(tif);

        TIFFFlushData(tif);

        _TIFFfree(tif_buf);

    } // function writeImageData

    const IntTypeMap &getTypeMap() const override
    {
        static const IntTypeMap tm = {
                {8+SAMPLEFORMAT_UINT, typeUInt8},
                {8+SAMPLEFORMAT_INT,  typeInt8},
                {16+SAMPLEFORMAT_UINT, typeUInt16},
                {16+SAMPLEFORMAT_INT, typeInt16},
                {32+SAMPLEFORMAT_UINT, typeUInt32},
                {32+SAMPLEFORMAT_INT, typeInt32},
                {32+SAMPLEFORMAT_IEEEFP, typeFloat}
        };
        return tm;
    } // function getTypeMap

    void setHeaderType(TiffHeader &header) const
    {
        //int mode = vHeader[0].bitsPerSample + vHeader[0].imageSampleFormat;
        int mode = getModeFromType(type);

        if (mode < 16)
            header.bitsPerSample = 8;
        else if (mode < 32)
            header.bitsPerSample = 16;
        else
            header.bitsPerSample = 32;

        header.imageSampleFormat = (uint16)mode - header.bitsPerSample;
    } // function setHeaderType

    void expandFile() override
    {
        /* Expansion of data has to be implemented at the same time with header
         * as image data has to be included in TIFF Directory when created */
    }
}; // class ImageIOTiff

StringVector tiffExts = {"tif", "tiff"};

REGISTER_IMAGE_IO(tiffExts, ImageIOTiff);

//#endif