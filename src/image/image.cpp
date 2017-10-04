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
#include "em/base/registry.h"
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
    *this = other;
} // Copy ctor Image

Image& Image::operator=(const Image &other)
{
    std::cout << "Assigning Image..." << std::endl;
    Array::operator=(other);
    implPtr->headers = other.implPtr->headers;
    return *this;
} //operator=

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

using ImageIOImplRegistry = ImplRegistry<ImageIOImpl>;

ImageIOImplRegistry * getRegistry()
{
    static ImageIOImplRegistry registry;

    return &registry;
}

bool em::registerImageIOImpl(const StringVector &extensions,
                             ImageIOImplBuilder builder)
{
    auto registry = getRegistry();
    for (auto ext: extensions)
        registry->registerImpl(ext, builder);

    return true;
} // function registerImageIOImpl


bool ImageIO::hasImpl(const std::string &extension)
{
    return getRegistry()->hasImpl(extension);
} // function hasIO

//ImageIO* ImageIO::get(const std::string &extension)
//{
//    auto builder = getRegistry()->getImplBuilder(extension);
//    return builder != nullptr ? builder() : nullptr;
//} // function getIO

ImageIO::ImageIO(const std::string &extOrName)
{
    auto builder = getRegistry()->getImplBuilder(extOrName);
    assert(builder!= nullptr);
    impl = builder();
} // Ctor ImageIO

ImageIO::~ImageIO()
{
    close();
    delete impl;
}// ImageIO ctor


ImageIOImpl::~ImageIOImpl()
{

}

void ImageIO::open(const std::string &path, const FileMode mode)
{
    impl->path = path;
    impl->fileMode = mode;

    impl->openFile();

    if (mode != TRUNCATE)
        impl->readHeader();
} // open

void ImageIO::close()
{
    if (impl != nullptr && impl->file != nullptr)
    {
        fclose(impl->file);
        impl->file = nullptr;
    }

    std::cout << "Close handle after" << std::endl;
}

void ImageIO::createFile(const ArrayDim &adim, ConstTypePtr type)
{
    if (impl->fileMode != TRUNCATE)
        THROW_ERROR("ImageIO::createFile can only be used with TRUNCATE mode.");

    // TODO: Check that the format supports this Type

    impl->dim = adim;
    impl->type = type;

    impl->writeHeader(); // write the main header of the file
    impl->expandFile();
}


void ImageIO::expandFile(const size_t ndim)
{
    // TODO: IMPLEMENT
} // function expandFile

ArrayDim ImageIO::getDimensions() const
{
    ASSERT_ERROR(impl == nullptr, "File has not been opened. ");

    return impl->dim;
}

void ImageIO::read(const size_t index, Image &image)
{
    // TODO: Validate that open has been previously called and succeeded

    ArrayDim adim = impl->dim;
    adim.n = 1; // Allocate for just one element

    ConstTypePtr imageType = image.getType();
    ConstTypePtr fileType = impl->type;

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

    std::cout << "DEBUG:ImageIO::read: imageType: " << *imageType << std::endl;
    std::cout << "DEBUG:ImageIO::read: " << *fileType << std::endl;

    // If the image has the same Type as the file
    // we do not need an intermediate buffer, we can read data
    // directly into the image memory
    // TODO: Check how this plays with Images in GPU memory

    auto& readImage = (sameType) ? image : impl->image;
    readImage.resize(adim, fileType);

    impl->readImageData(index, readImage);

    // TODO: Check swap
    //swap per page
    //if (swap)
    //    swapPage(page, readsize, datatype);
    // cast to T per page
    //castPage2T(page, MULTIDIM_ARRAY(data) + haveread_n, datatype, readsize_n);
}

void ImageIO::read(const ImageLocation &location, Image &image)
{
    std::cout << " open in read: " << location.path << std::endl;
    open(location.path);
    // FIXME: Now only reading the first image in the location range
    std::cout << " read(location.start: " << location.index << ")" << std::endl;
    read(location.index, image);
    close();
    std::cout << " Close file" << std::endl;

} // ImageIO::read

void ImageIO::write(const size_t index, const Image &image)
{
    auto type = impl->type;

    std::cerr << "ImageIO::write: type: " << type << std::endl;
    std::cerr << "ImageIO::write: image.getType(): " << image.getType() << std::endl;

    ASSERT_ERROR(image.getType() != type,
                 "Type cast not implemented. Now image should have the same "
                 "type.")

    impl->writeImageData(index, image);
} // function ImageIO::write

size_t ImageIOImpl::getPadSize() const
{
    return pad;
} // function ImageIOImpl::getPadSize

const char * ImageIOImpl::getOpenMode(FileMode mode) const
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
} // function ImageIOImpl::getOpenMode


void ImageIOImpl::openFile()
{
    std::cout << "ImageIOImpl::openFile: mode: " << getOpenMode(fileMode) <<
              "file: " << path << std::endl;

    file = fopen(path.c_str(), getOpenMode(fileMode));

    if (file == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file: ") + path);
} // function ImageIOImpl::openFile

void ImageIOImpl::closeFile()
{
    fclose(file);
} // function ImageIOImpl::closeFile

void ImageIOImpl::expandFile()
{
    // Compute the size of one item, taking into account its x, y, z dimensions
    // and the size of the type that will be used
    size_t itemSize = dim.getItemSize() * type->getSize();

    // Compute the total size of the file taking into account the general header
    // size and the size of all items (including extra padding per item)
    size_t fileSize = getHeaderSize() + (itemSize + getPadSize()) * dim.n;

    std::cout << "ImageIO::createFile: fileSize: " << fileSize << std::endl;
    std::cout << "                     itemSize: " << itemSize << std::endl;
    std::cout << "                     getHeaderSize(): " << getHeaderSize() << std::endl;
    std::cout << "                     getPadSize(): " << getPadSize() << std::endl;

    File::expand(file, fileSize);
    fflush(file);

} // function ImageIOImpl::expandFile

void ImageIOImpl::readImageHeader(const size_t index, Image &image)
{

}

void ImageIOImpl::writeImageHeader(const size_t index, const Image &image)
{

}

void ImageIOImpl::readImageData(const size_t index, Image &image)
{
    // NOTE: If in a future we want to read more than one continuous image
    // we could just move point the padding space between images
    // For now, we are just positioning the pointer to the place where
    // the required image is stored. Basically we need to shift the pointer
    // HEADER_SIZE + (IMAGE_SIZE + PAD_SIZE) * (IMG_INDEX - 1)
    size_t itemSize = dim.getItemSize() * type->getSize();
    size_t itemPos = getHeaderSize() + (itemSize + getPadSize()) * (index - 1);

    std::cerr << "DEBUG: fseeking to " << itemPos << std::endl;

    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    // FIXME: change this to read by chunks when we change this
    // approach, right now only read a big chunk of one item size
    std::cerr << "DEBUG: reading " << itemSize << " bytes." << std::endl;

    if (fread(image.getDataPointer(), itemSize, 1, file) != 1)
        THROW_SYS_ERROR("Could not 'fread' data from file. ");
}

void ImageIOImpl::writeImageData(const size_t index, const Image &image)
{
    auto data = image.getDataPointer();
    size_t itemSize = dim.getItemSize() * type->getSize();
    size_t itemPos = getHeaderSize() + (itemSize + getPadSize()) * (index - 1);

    std::cerr << "ImageIOImpl::write: itemPos: " << itemPos << std::endl;
    std::cerr << "ImageIOImpl::write: itemSize: " << itemSize << std::endl;


    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    fwrite(data, itemSize, 1, file);

} // function ImageIOImpl::write

