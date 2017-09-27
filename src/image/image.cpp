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

std::map<std::string, const ImageIO*> ImageIO::iomap;

bool ImageIO::set(const ImageIO *reader)
{
    for (auto ext: reader->getExtensions())
    {
        iomap[ext] = reader;
    }

    iomap[reader->getName()] = reader;
    return true;
} // function registerIO

bool ImageIO::has(const std::string &extension)
{
    auto it = iomap.find(extension);
    return it != iomap.end();
} // function hasIO

ImageIO* ImageIO::get(const std::string &extension)
{
    if (!ImageIO::has(extension))
        return nullptr;

    return iomap[extension]->create();
} // function getIO

ImageIO::~ImageIO()
{
    close();
    delete handler;
}// ImageIO ctor

void ImageIO::open(const std::string &path, FileMode const mode)
{
    // Keep a mapping between our numeric FileMode and the string modes
    const char * openMode = "r";

    switch (mode)
    {
        case ImageIO::READ_WRITE:
            openMode = "r+"; break;  // FIXME: File must exits
        case ImageIO::TRUNCATE:
            openMode = "w"; break;
    }

    // We create the handler the first time that we enter this point.
    // It makes sense to create the handler in the constructor
    // but we cannot not do that because it is a virtual function
    if (handler == nullptr)
        handler = createHandler();

    handler->path = path;
    handler->file = fopen(path.c_str(), openMode);

    if (handler->file == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file '") + path);

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
    //TODO: IMPLEMENT
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
    size_t itemPosition = getHeaderSize() + (itemSize + getPadSize()) * (index - 1);

    std::cerr << "DEBUG: fseeking to " << itemPosition << std::endl;

    if (fseek(handler->file, itemPosition, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    // FIXME: change this to read by chunks when we change this
    // approach, right now only read a big chunk of one item size
    std::cerr << "DEBUG: reading " << itemSize << " bytes." << std::endl;

    if (fread(data, itemSize, 1, handler->file) != 1)
        THROW_SYS_ERROR("Could not 'fread' data from file. ");

    // TODO
    //swap per page
    //if (swap)
    //    swapPage(page, readsize, datatype);
    // cast to T per page
    //castPage2T(page, MULTIDIM_ARRAY(data) + haveread_n, datatype, readsize_n);
}

void ImageIO::write(const size_t index, const Image &image)
{

}

void ImageIO::read(const ImageLocation &location, Image &image)
{
    std::cout << " open: " << location.path << std::endl;
    open(location.path);
    // FIXME: Now only reading the first image in the location range
    std::cout << " read(location.start: " << location.index << std::endl;
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


#include "em/image/image_spider_priv.h"
REGISTER_IMAGE_IO(ImageSpiderIO);

#include "em/image/image_mrc_priv.h"
REGISTER_IMAGE_IO(ImageMrcIO);

#include "em/image/image_tiff_priv.h"
REGISTER_IMAGE_IO(ImageTiffIO);
