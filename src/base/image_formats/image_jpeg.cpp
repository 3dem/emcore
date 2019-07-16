#include <iomanip>
#include <setjmp.h>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/base/image.h"
#include "em/base/image_priv.h"

#include "jpeglib.h"

using namespace em;





/**
 * Inherit properties from base ImageFile::Impl and add information
 * specific for JPEG format
 */
class ImageIOJpeg: public em::ImageFile::Impl
{
public:
    jpeg_decompress_struct dInfo;
    jpeg_compress_struct cInfo;
    jpeg_error_mgr jError;
    jmp_buf jBuf;


    virtual void readHeader() override
    {
        //creating decompress info
        jpeg_create_decompress(&dInfo);

        dInfo.err = jpeg_std_error(&jError);
        if (setjmp(jBuf))
        {
            jpeg_destroy_decompress(&dInfo);
            THROW_SYS_ERROR(std::string("JPEG internal error."));
        }

        //specify data source
        jpeg_stdio_src(&dInfo, file);
        //read file parameters with jpeg_read_header()
        jpeg_read_header(&dInfo, TRUE);

        if (dInfo.jpeg_color_space != JCS_GRAYSCALE)
        {
            jpeg_destroy_decompress(&dInfo);
            THROW_SYS_ERROR(std::string("Unsupported color space."));
        }

        dim.x = dInfo.image_width;
        dim.y = dInfo.image_height;
        dim.z = 1;
        dim.n = 1;

        type = getTypeFromMode(dInfo.data_precision > 8 ? 2 : 1);
    } // function readHeader

    virtual void writeHeader() override
    {
        cInfo.err = jpeg_std_error(&jError);
        jpeg_create_compress(&cInfo);
        jpeg_stdio_dest(&cInfo, file);
    } // function writeHeader


    virtual void readImageData(const size_t index, Image& image) override
    {
        if (index != 1)
            THROW_SYS_ERROR(std::string("Index out of bounds."));

        jpeg_start_decompress(&dInfo);

        int rowStride, i;
        rowStride = dInfo.output_width * dInfo.output_components * type.getSize();
        i = 0;
        auto data = static_cast<uint8_t*>(image.getData());
        JSAMPROW row;

        while(dInfo.output_scanline < dInfo.image_height)
        {
            row = static_cast<JSAMPROW>(data + i*rowStride);
            jpeg_read_scanlines(&dInfo, &row, 1);
            i++;
        }

        jpeg_finish_decompress(&dInfo);
    } // function readImageData

    virtual void writeImageData(const size_t index, const Image &image) override
    {
        if (index != 1)
            THROW_SYS_ERROR(std::string("Index out of bounds."));

        cInfo.image_width = dim.x;
        cInfo.image_height = dim.y;
        cInfo.input_components = 1;
        cInfo.in_color_space = JCS_GRAYSCALE;

        jpeg_set_defaults(&cInfo);

        jpeg_start_compress(&cInfo, TRUE);

        int rowStride, i;
        rowStride = cInfo.image_width * cInfo.input_components * type.getSize();
        i = 0;
        auto data = static_cast<const uint8_t*>(image.getData());
        JSAMPLE row[rowStride];
        JSAMPROW buff[1];
        buff[0] = row;

        while(cInfo.next_scanline < cInfo.image_height)
        {
            std::memcpy(row, data + i*rowStride, rowStride);
            jpeg_write_scanlines(&cInfo, buff, 1);
            i++;
        }

        jpeg_finish_compress(&cInfo);

    } // function writeImageHeader

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {
               {1, typeUInt8},
               {2, typeUInt16}
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

    virtual void closeFile() override
    {
        em::ImageFile::Impl::closeFile();

        jpeg_destroy_decompress(&dInfo);
        jpeg_destroy_compress(&cInfo);
    }

    ~ImageIOJpeg()
    {

    }

}; // class ImageIOPng

StringVector jpegExts = {"jpg", "jpeg"};

REGISTER_IMAGE_IO(jpegExts, ImageIOJpeg);

