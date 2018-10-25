#include <iomanip>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/base/image.h"
#include "em/base/image_priv.h"

#include "png.h"

using namespace em;





/**
 * Inherit properties from base ImageIO::Impl and add information
 * specific for PNG format (e.g, the ImagicHeader struct)
 */
class ImageIOPng: public em::ImageIO::Impl
{
public:
    png_infop readInfoPtr = nullptr;
    png_structp pngReadPtr = nullptr;
    png_infop writeInfoPtr = nullptr;
    png_structp pngWritePtr = nullptr;

    virtual void readHeader() override
    {
        // Read the first 8 bytes and make sure the match the PNG signatures bytes:
        u_char sig[8];
        if (fread(sig, 1, 8, file) < 1)
            THROW_SYS_ERROR(std::string("Error reading PNG signatures in file: ") + path);

        if (!png_check_sig(sig, 8))
            THROW_SYS_ERROR(std::string("Incorrect PNG signatures."));

        pngReadPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

        if (!pngReadPtr)
            THROW_SYS_ERROR(std::string("Error allocating png read struct"));

        readInfoPtr = png_create_info_struct(pngReadPtr);

        if (!readInfoPtr)
        {
            png_destroy_read_struct(&pngReadPtr, nullptr, nullptr);
            THROW_SYS_ERROR(std::string("Error allocating png info struct"));
        }

        //generic error-handling (www.libpng.org/pub/png/book/chapter13.html)
        if (setjmp(png_jmpbuf(pngReadPtr)))
        {
            png_destroy_read_struct(&pngReadPtr, &readInfoPtr, nullptr);
            pngReadPtr = nullptr;
            readInfoPtr = nullptr;
            THROW_SYS_ERROR(std::string("Generic error-handling"));
        }

        png_init_io(pngReadPtr, file);
        png_set_sig_bytes(pngReadPtr, 8);
        png_read_info(pngReadPtr, readInfoPtr);

        png_byte colorType = png_get_color_type(pngReadPtr, readInfoPtr);

        if (colorType != PNG_COLOR_TYPE_GRAY)
            THROW_SYS_ERROR(std::string("Unsupported color type."));

        dim.x = (size_t) png_get_image_width(pngReadPtr, readInfoPtr);
        dim.y = (size_t) png_get_image_height(pngReadPtr, readInfoPtr);
        dim.z = 1;
        dim.n = 1;

        type = getTypeFromMode(png_get_bit_depth(pngReadPtr, readInfoPtr));
    } // function readHeader

    virtual void writeHeader() override
    {
        if (pngWritePtr == nullptr)
        {
            pngWritePtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            if (pngWritePtr == nullptr)
                THROW_SYS_ERROR(std::string("Error allocating png write struct."));
        }

        if (writeInfoPtr == nullptr)
        {
            writeInfoPtr = png_create_info_struct(pngWritePtr);
            if (writeInfoPtr == nullptr)
                THROW_SYS_ERROR(std::string("Error allocating png info struct."));
        }

        //generic error-handling (www.libpng.org/pub/png/book/chapter13.html)
        if (setjmp(png_jmpbuf(pngWritePtr)))
        {
            png_destroy_write_struct(&pngWritePtr, &writeInfoPtr);
            pngWritePtr = nullptr;
            writeInfoPtr = nullptr;
            THROW_SYS_ERROR(std::string("Generic error-handling"));
        }

        png_init_io(pngWritePtr, file);

        int mode = getModeFromType(type);

        png_set_IHDR(pngWritePtr,
                     writeInfoPtr,
                     dim.x,
                     dim.y,
                     mode == 16 ? 16: 8,
                     PNG_COLOR_TYPE_GRAY,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE);

        png_write_info(pngWritePtr, writeInfoPtr);
    } // function writeHeader


    virtual void readImageData(const size_t index, Image& image) override
    {
        png_byte bitDepth = png_get_bit_depth(pngReadPtr, readInfoPtr);

        if (bitDepth < 8)// for PNG_COLOR_TYPE_GRAY
            png_set_expand_gray_1_2_4_to_8(pngReadPtr);

        auto data1 = static_cast<uint8_t *>(image.getData());
        png_uint_32 i, rowBytes;
        png_bytep row;

        rowBytes = png_get_rowbytes(pngReadPtr, readInfoPtr);

        for (i = 0; i < dim.y; i++)
        {
            row = static_cast<png_bytep>(data1 + i*rowBytes);
            png_read_row(pngReadPtr, row, row);
        }

        png_read_end(pngReadPtr, readInfoPtr);

    } // function readImageData

    virtual void writeImageData(const size_t index, const Image &image) override
    {
        auto data1 = static_cast<const uint8_t*>(image.getData());
        png_uint_32 i, rowBytes = dim.x * (getModeFromType(type) == 16 ? 2 : 1);
        png_byte row[rowBytes];

        for (i = 0; i < dim.y; i++)
        {
            std::memcpy(row, data1 + i*rowBytes, rowBytes);
            png_write_row(pngWritePtr, row);
        }

        png_write_end(pngWritePtr, writeInfoPtr);

    } // function writeImageHeader

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {
               {1, typeUInt8},
               {2, typeUInt8},
               {4, typeUInt8},
               {8, typeUInt8},
               {16, typeInt16}
        };

        return tm;
    } // function getTypeMap

    virtual void toStream(std::ostream &ostream, int verbosity) const override
    {

    } // function toStream

    virtual void expandFile() override
    {
        //FIXME [hv]: Consider expand file(may be imposible).
    }

    ~ImageIOPng()
    {
       if (pngReadPtr != nullptr && readInfoPtr !=nullptr)
            png_destroy_read_struct(&pngReadPtr, &readInfoPtr, nullptr);

        if (pngWritePtr != nullptr && writeInfoPtr != nullptr)
            png_destroy_write_struct(&pngWritePtr, &writeInfoPtr);
    }

}; // class ImageIOPng

StringVector pngExts = {"png"};

REGISTER_IMAGE_IO(pngExts, ImageIOPng);

