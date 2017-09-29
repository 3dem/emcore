//
// Created by josem on 2017-06-09.
//

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

#include "em/base/error.h"
#include "em/base/type.h"
#include "em/base/array.h"
#include "em/os/file.h"
#include "em/image/image_priv.h"



using namespace em;


// ===================== ImageImpl Implementation =======================

class ImageImpl
{
public:
    // headers[0] will be the main header and
    // the rest one per image
    std::vector<ObjectDict> headers;

    //static std::map<std::string, ImageWriter*> writers;

    ImageImpl()
    {
        // Create at least one map to store the header of the main image
        headers.push_back(ObjectDict());
    }
};


// ===================== Image Implementation =======================

Image::Image(): Array()
{
    implPtr = new ImageImpl();
} // empty Ctor

Image::Image(const ArrayDim &adim, ConstTypePtr type): Array(adim, type)
{
    implPtr = new ImageImpl();
    // Type should be not null
    // (another option could be assume float or double by default)
    assert(type != nullptr);
} // Ctor for ArrayDim and ConstTypePtr

Image::Image(const Image &other): Array(other)
{
    implPtr = new ImageImpl();
} // Copy ctor Image

Image::~Image()
{
    std::cout << "this: " << this << " ~Image()" << std::endl;
    delete implPtr;
} // Dtor

ObjectDict& Image::getHeader(size_t index)
{
    return implPtr->headers[index];
}

void Image::toStream(std::ostream &ostream) const
{
    ostream << "Dimensions: " << getDimensions() << std::endl;
    ostream << "Type: " << *getType() << std::endl;
    ostream << "Header: " << std::endl;
    for (auto& x: implPtr->headers[0]) {
        std::cout << x.first << ": " << x.second << std::endl;
    }
    // Array::toStream(ostream);
}

std::ostream& em::operator<< (std::ostream &ostream, const em::Image &image)
{
    image.toStream(ostream);
    return ostream;
}


// ===================== ImageIO Implementation =======================

std::map<std::string, const ImageIO*>* ImageIO::iomap;

bool ImageIO::set(const ImageIO *reader)
{
    static std::map<std::string, const ImageIO*> localmap;
    iomap = &localmap;

    for (auto ext: reader->getExtensions())
        (*iomap)[ext] = reader;

    (*iomap)[reader->getName()] = reader;
    return true;
} // function registerIO

bool ImageIO::has(const std::string &extension)
{
    auto it = (*iomap).find(extension);
    return it != (*iomap).end();
} // function hasIO

ImageIO* ImageIO::get(const std::string &extension)
{
    if (!ImageIO::has(extension))
        return nullptr;

    return (*iomap)[extension]->create();
} // function getIO

ImageIO::~ImageIO()
{
    close();
    delete handler;
}// ImageIO ctor

void ImageIO::open(const std::string &path, const FileMode mode)
{
    // It makes sense to create the handler in the constructor
    // but we cannot not do that because it is a virtual function

    // We create the handler the first time that we enter this point.
    std::cout << "just before openFile" << std::endl;
    if (handler == nullptr)
        handler = createHandler();

    handler->path = path;
    handler->fileMode = mode;

    handler->openFile();
    readHeader();
} // open

void ImageIO::close()
{
    if (handler != nullptr && handler->file != nullptr)
    {
        fclose(handler->file);
        handler->file = nullptr;
    }

    std::cout << "Close handle after" << std::endl;
}

void ImageIO::createFile(const ArrayDim &adim, ConstTypePtr type)
{
    if (handler->fileMode != ImageIO::TRUNCATE)
        THROW_ERROR("ImageIO::createFile can only be used with TRUNCATE mode.");

    // TODO: Check that the format supports this Type

    handler->dim = adim;
    handler->type = type;

    writeHeader(); // write the main header of the file

    // Compute the size of one item, taking into account its x, y, z dimensions
    // and the size of the type that will be used
    size_t itemSize = adim.getItemSize() * type->getSize();

    // Compute the total size of the file taking into account the general header
    // size and the size of all items (including extra padding per item)
    size_t fileSize = getHeaderSize() + (itemSize + getPadSize()) * adim.n;

    File::expand(handler->file, fileSize);

} // function createFile

void ImageIO::expandFile(const size_t ndim)
{
    // TODO: IMPLEMENT
} // function expandFile

ArrayDim ImageIO::getDimensions() const
{
    ASSERT_ERROR(handler == nullptr, "File has not been opened. ");

    return handler->dim;
}

void ImageIO::read(const size_t index, Image &image)
{
    // TODO: Validate that open has been previously called and succeeded

    ArrayDim adim = handler->dim;
    adim.n = 1; // Allocate for just one element

    ConstTypePtr imageType = image.getType();
    ConstTypePtr fileType = handler->type;

    // If the image already has a defined Type, we should respect that
    // one and then convert from the data read from disk.
    // If the image
    if (imageType == nullptr)
    {
        image.resize(adim, fileType);
        imageType = fileType;
    }
    else
        image.resize(adim);

    bool sameType = (imageType == fileType);
    void * data = nullptr;

    std::cout << "DEBUG:read: imageType: " << *imageType << std::endl;
    std::cout << "DEBUG:read: fileType: " << *fileType << std::endl;

    // If the image has the same Type as the file
    // we do not need an intermediate buffer, we can read data
    // directly into the image memory
    // TODO: Check how this plays with Images in GPU memory
    if (sameType)
    {
        data = image.getDataPointer();
    }
    else
    {
        // TODO: image one item is too big, we could think of a
        // smaller chunk of the image
        handler->image.resize(adim, fileType);
        data = handler->image.getDataPointer();
    }

    // NOTE: If in a future we want to read more than one continuous image
    // we could just move point the padding space between images
    // For now, we are just positioning the pointer to the place where
    // the required image is stored. Basically we need to shift the pointer
    // HEADER_SIZE + (IMAGE_SIZE + PAD_SIZE) * (IMG_INDEX - 1)
    size_t itemSize = adim.getItemSize() * fileType->getSize();
    size_t itemPos = getHeaderSize() + (itemSize + getPadSize()) * (index - 1);

    std::cerr << "DEBUG: fseeking to " << itemPos << std::endl;

    if (fseek(handler->file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    // FIXME: change this to read by chunks when we change this
    // approach, right now only read a big chunk of one item size
    std::cerr << "DEBUG: reading " << itemSize << " bytes." << std::endl;

    if (fread(data, itemSize, 1, handler->file) != 1)
        THROW_SYS_ERROR("Could not 'fread' data from file. ");

    // TODO: Check swap
    //swap per page
    //if (swap)
    //    swapPage(page, readsize, datatype);
    // cast to T per page
    //castPage2T(page, MULTIDIM_ARRAY(data) + haveread_n, datatype, readsize_n);
}

void ImageIO::write(const size_t index, const Image &image)
{
    auto type = handler->type;

    ASSERT_ERROR(image.getType() != type,
                 "Type cast not implemented. Now image should have the same "
                 "type.")

    auto data = handler->image.getDataPointer();

    size_t itemSize = handler->dim.getItemSize() * type->getSize();
    size_t itemPos = getHeaderSize() + (itemSize + getPadSize()) * (index - 1);

    if (fseek(handler->file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    fwrite(data, itemSize, 1, handler->file);

        // void writeData(FILE* fimg, size_t offset, DataType wDType, size_t datasize_n, CastWriteMode castMode = CW_CAST)
//        size_t dTypeSize = gettypesize(wDType);
//        size_t datasize = datasize_n * dTypeSize;
//        size_t ds2Write = rw_max_page_size;
//        size_t dsN2Write = rw_max_page_size / dTypeSize;
//        size_t rw_max_n = dsN2Write;
//
//        char* fdata;
//        double min0 = 0, max0 = 0;
//
//        if (wDType == myT() && castMode == CW_CONVERT)
//            castMode = CW_CAST;
//
//        if (castMode != CW_CAST)
//            data.computeDoubleMinMaxRange(min0, max0, offset, datasize_n);
//
//        if (datasize > rw_max_page_size)
//            fdata = (char *) askMemory(rw_max_page_size * sizeof(char));
//        else
//            fdata = (char *) askMemory(datasize * sizeof(char));
//
//        for (size_t writtenDataN = 0; writtenDataN < datasize_n; writtenDataN +=
//                                                                         rw_max_n)
//        {
//
//            if (writtenDataN + rw_max_n > datasize_n)
//            {
//                dsN2Write = datasize_n - writtenDataN;
//                ds2Write = dsN2Write * dTypeSize;
//            }
//
//            if (castMode == CW_CAST)
//                castPage2Datatype(MULTIDIM_ARRAY(data) + offset + writtenDataN, fdata,
//                                  wDType, dsN2Write);
//            else
//                castConvertPage2Datatype(MULTIDIM_ARRAY(data) + offset + writtenDataN,
//                                         fdata, wDType, dsN2Write, min0, max0, castMode);
//
//            //swap per page
//            if (swapWrite)
//                swapPage(fdata, ds2Write, wDType);
//
//            fwrite(fdata, ds2Write, 1, fimg);
//        }
//        freeMemory(fdata, rw_max_page_size);
} // function write

void ImageIO::read(const ImageLocation &location, Image &image)
{
    std::cout << " open in read: " << location.path << std::endl;
    open(location.path);
    // FIXME: Now only reading the first image in the location range
    std::cout << " read(location.start: " << location.index << ")" << std::endl;
    read(location.index, image);
    close();
    std::cout << " Close file" << std::endl;

}

size_t ImageIO::getPadSize() const
{
    return handler->pad;
}


ImageHandler* ImageIO::createHandler()
{
    return new ImageHandler;
} // createHandler

const char * ImageHandler::getOpenMode(FileMode mode) const
{
    const char * openMode = "r";

    switch (mode)
    {
        case ImageIO::READ_WRITE:
            openMode = "r+"; break;  // FIXME: File must exits
        case ImageIO::TRUNCATE:
            openMode = "w"; break;
    }

    return openMode;
}

void ImageHandler::openFile()
{
    file = fopen(path.c_str(), getOpenMode(fileMode));

    if (file == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file '") + path);
}
