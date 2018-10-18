//
// Created by josem on 2017-06-09.
//

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>

#include "em/base/error.h"
#include "em/base/log.h"
#include "em/base/type.h"
#include "em/base/array.h"
#include "em/base/registry.h"
#include "em/os/filesystem.h"
#include "em/base/image_priv.h"


using namespace em;


// ===================== ImageImpl Implementation =======================

class Image::Impl
{
public:
    // headers[0] will be the main header and
    // the rest one per image
    std::vector<ObjectDict> headers;

    //static std::map<std::string, ImageWriter*> writers;

    Impl()
    {
        // Create at least one map to store the header of the main image
        headers.push_back(ObjectDict());
    }
}; // class Image::Impl


// ================== ImageLocation Implementation ====================

ImageLocation::ImageLocation(const std::string &path, size_t index):
index(index), path(path)
{}

bool ImageLocation::operator==(const ImageLocation &other) const
{
    return (index == other.index && path == other.path);
}

bool ImageLocation::operator!=(const ImageLocation &other) const
{
    return !(*this == other);
}

std::string ImageLocation::toString() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
} // function ArrayDim.toString

std::ostream& em::operator<< (std::ostream &ostream, const ImageLocation &loc)
{
    ostream << "(" << loc.path << ", ";

    if (loc.index == ImageLocation::ALL)
        ostream << "ALL" << ")";
    else
        ostream << loc.index << ")";

    return ostream;
} // operator >> for ArrayDim


// ===================== Image Implementation =======================

Image::Image(): Array()
{
    impl = new Impl();
} // empty Ctor

Image::Image(const ArrayDim &adim, const Type & type): Array(adim, type)
{
    impl = new Impl();
    // Type should be not null
    // (another option could be assume float or double by default)
    ASSERT_ERROR(type.isNull(), "Image type can not be null. ");
} // Ctor for ArrayDim and const Type &

Image::Image(const Image &other): Array(other)
{
    impl = new Impl();
    *this = other;
} // Copy ctor Image

Image& Image::operator=(const Image &other)
{
    Array::operator=(other);
    impl->headers = other.impl->headers;
    return *this;
} //operator=

Image::~Image()
{
    delete impl;
} // Dtor

ObjectDict& Image::getHeader(size_t index)
{
    return impl->headers[index];
}

void Image::toStream(std::ostream &ostream) const
{
    ostream << " -- Image info --" << std::endl;
    ostream << "Dimensions: " << getDim() << std::endl;
    ostream << "Type: " << getType() << std::endl;
    ostream << "Header: " << std::endl;
    for (auto& x: impl->headers[0]) {
        std::cout << x.first << ": " << x.second << std::endl;
    }
    // Array::toStream(ostream);
}

std::ostream& em::operator<< (std::ostream &ostream, const em::Image &image)
{
    image.toStream(ostream);
    return ostream;
}

std::istream& em::operator>>(std::istream &istream, em::Image &image)
{
    THROW_ERROR("Reading Image from std::istream is NOT IMPLEMENTED.");
}

void Image::read(const ImageLocation &location)
{
    ImageIO imgio;
    imgio.open(location.path);
    // FIXME: Now only reading the first image in the location range
    imgio.read(location.index, *this);
    imgio.close();
} // function Image::read

void Image::write(const ImageLocation &location) const
{
    ImageIO imgio;

    if (Path::exists(location.path))
        imgio.open(location.path,  File::Mode::READ_WRITE);
    else
    {
        imgio.open(location.path,  File::Mode::TRUNCATE);
        imgio.createFile(getDim(), getType());
    }

    imgio.write(location.index, *this);
    imgio.close();
} // function Image::write

// ===================== ImageIO Implementation =======================

using ImageIOImplRegistry = ImplRegistry<ImageIO::Impl>;

ImageIOImplRegistry * getImageIORegistry()
{
    static ImageIOImplRegistry registry;
    return &registry;
} // function getImageIORegistry

bool em::ImageIO::registerImpl(const StringVector &extOrNames,
                             ImageIO::ImplBuilder builder)
{
    return getImageIORegistry()->registerImpl(extOrNames, builder);
} // function registerImageIOImpl

ImageIO::FormatTypes ImageIO::getFormatTypes()
{
    FormatTypes dict;

    for (const auto& kv: getImageIORegistry()->getUniqueMap())
    {
        dict[kv.first] = {};
        auto& vector = dict[kv.first];

        auto impl = kv.second();

        for (const auto& kv2: impl->getTypeMap())
            vector.push_back(kv2.second);

        std::sort(vector.begin(), vector.end(),
                  [](const Type& t1, const Type& t2)
                  {
                      if (t1.getSize() == t2.getSize())
                      {
                          return t1.getName() < t2.getName();
                      }
                      else return t1.getSize() < t2.getSize();
                  });
    }

    return dict;
}

bool ImageIO::hasImpl(const std::string &extension)
{
    return getImageIORegistry()->hasImpl(extension);
} // function hasIO

ImageIO::ImageIO()
{
    impl = nullptr;
} // Ctor ImageIO

ImageIO::ImageIO(const std::string &extOrName)
{
    impl = getImageIORegistry()->buildImpl(extOrName);
} // Ctor ImageIO

ImageIO::~ImageIO()
{
    close();
    delete impl;
}// ImageIO ctor


ImageIO::Impl::~Impl()
{
}

void ImageIO::open(const std::string &path, const File::Mode mode)
{
    delete impl;  // Does it make sense to reuse impl?
    impl = getImageIORegistry()->buildImpl(Path::getExtension(path));
    impl->path = path;
    impl->fileMode = mode;

    // If the file does not exists and mode is  File::Mode::READ_WRITE
    // switch automatically to TRUNCATE mode
    if (mode ==  File::Mode::READ_WRITE and !Path::exists(path))
        impl->fileMode =  File::Mode::TRUNCATE;

    impl->openFile();

    if (impl->fileMode !=  File::Mode::TRUNCATE)
        impl->readHeader();
} // function ImageIO.open

void ImageIO::close()
{
    if (impl != nullptr)
    {
        impl->closeFile();
    }
} // function ImageIO.close

void ImageIO::createFile(const ArrayDim &adim, const Type & type)
{
    ASSERT_ERROR(type.isNull(), "Input type can not be null. ");
    ASSERT_ERROR(impl->fileMode !=  File::Mode::TRUNCATE,
                 "ImageIO::createFile can only be used with TRUNCATE mode.");
    // TODO: Check that the format supports this Type
    impl->dim = adim;
    impl->type = type;
    impl->writeHeader(); // write the main header of the file
    impl->expandFile();
} // function ImageIO.createFile


void ImageIO::expandFile(const size_t ndim)
{
    // TODO: IMPLEMENT
} // function expandFile

ArrayDim ImageIO::getDim() const
{
    ASSERT_ERROR(impl == nullptr, "File has not been opened. ");

    return impl->dim;
}

void ImageIO::read(size_t index, Image &image)
{
    ArrayDim adim = getDim(); // This will check that the file was open
    ASSERT_ERROR(index > adim.n, "Invalid index");

    // TODO: Allow to read more than one image
    if (index == ImageLocation::ALL)
        index = ImageLocation::FIRST;
    adim.n = 1; // Allocate for just one element

    auto imageType = image.getType();
    auto fileType = impl->type;

    // If the image already has a defined Type, we should respect that
    // one and then convert from the data read from disk.
    // If the image
    if (imageType.isNull())
    {
        image.resize(adim, fileType);
        imageType = fileType;
    }
    else
        image.resize(adim);

    bool sameType = (imageType == fileType);
    void * data = nullptr;

    // If the image has the same Type as the file
    // we do not need an intermediate buffer, we can read data
    // directly into the image memory
    // TODO: Check how this plays with Images in GPU memory

    auto& readImage = (sameType) ? image : impl->image;
    readImage.resize(adim, fileType);

    impl->readImageData(index, readImage);

    if (impl->swap)
        Type::swapBytes(readImage.getData(), adim.getItemSize(),
                        fileType.getSize());

    // If we have read the image into the internal buffer image due to
    // a different datatype, we need to cast now to the output image
    if (!sameType)
        image.copy(impl->image);
} // function ImageIO::read

void ImageIO::write(size_t index, const Image &image)
{
    auto& type = impl->type;

    // FIXME: Check what to do with ALL as index
    if (index == ImageLocation::ALL)
        index = ImageLocation::FIRST;

    if (type.isNull())
        // FIXME: I think we don't need to always create the file, check it!!!
        createFile(image.getDim(), image.getType());

    ASSERT_ERROR(image.getType() != type,
                 "Type cast not implemented. Now image should have the same "
                 "type.")

    impl->writeImageData(index, image);
} // function ImageIO::write

void ImageIO::toStream(std::ostream &ostream, int verbosity) const
{
    if (verbosity > 0)
    {
        ostream << " --- File:  " << impl->path << " ---" << std::endl
                << "Dimensions: " << impl->dim << " " << impl->type << std::endl
                << "Header size: " << impl->getHeaderSize() << " (main) "
                << impl->getPadSize() << " (per image),  "
                << "Swap: " << impl->swap << std::endl;

        if (verbosity > 1)
            impl->toStream(std::cout, verbosity);
    }
}

std::ostream& em::operator<< (std::ostream &ostream, const em::ImageIO &imageIO)
{
    imageIO.toStream(ostream);
    return ostream;
}

size_t ImageIO::Impl::getHeaderSize() const
{
    return 0;
} // function ImageIO::Impl::getHeaderSize

size_t ImageIO::Impl::getPadSize() const
{
    return pad;
} // function ImageIO::Impl::getPadSize

size_t ImageIO::Impl::getImageSize() const
{
    return dim.getItemSize() * type.getSize() + getPadSize();
} // function ImageIO::Impl::getImageSize

const char * ImageIO::Impl::getModeString() const
{
    return File::modeToString(fileMode);
} // function ImageIO::Impl::getModeString

void ImageIO::Impl::openFile()
{
    file = fopen(path.c_str(), getModeString());

    if (file == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file: ") + path);
} // function ImageIO::Impl::openFile

void ImageIO::Impl::closeFile()
{
    if (file != nullptr)
    {
        fclose(file);
        file = nullptr;
    }
} // function ImageIO::Impl::closeFile

void ImageIO::Impl::expandFile()
{
    // Compute the size of one item, taking into account its x, y, z dimensions
    // and the size of the type that will be used
    size_t itemSize = getImageSize();

    // Compute the total size of the file taking into account the general header
    // size and the size of all items (including extra padding per item)
    size_t fileSize = getHeaderSize() + itemSize * dim.n;

    File::resize(file, fileSize);
    fflush(file);

} // function ImageIO::Impl::expandFile

void ImageIO::Impl::readImageHeader(const size_t index, Image &image)
{

}

void ImageIO::Impl::writeImageHeader(const size_t index, const Image &image)
{

}

void ImageIO::Impl::readImageData(const size_t index, Image &image)
{
    size_t itemSize = getImageSize(); // Size of an item containing the padSize
    size_t padSize = getPadSize();
    size_t readSize = itemSize - padSize;
    // Compute the position of the item data in the file given its size
    size_t itemPos = getHeaderSize() + itemSize * (index - 1) + padSize;

    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    // FIXME: change this to read by chunks when we change this
    // approach, right now only read a big chunk of one item size

    if (::fread(image.getData(), readSize, 1, file) != 1)
        THROW_SYS_ERROR("Could not 'fread' data from file. ");

}

void ImageIO::Impl::writeImageData(const size_t index, const Image &image)
{
    size_t itemSize = getImageSize();
    size_t padSize = getPadSize();
    size_t writeSize = itemSize - padSize;
    size_t itemPos = getHeaderSize() + itemSize * (index - 1) + padSize;

    std::cerr << "ImageIO::Impl::write: itemPos: " << itemPos << std::endl;
    std::cerr << "ImageIO::Impl::write: itemSize: " << itemSize << std::endl;

    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR("Could not 'fseek' in file. ");

    fwrite(image.getData(), writeSize, 1, file);

} // function ImageIO::Impl::write

Type ImageIO::Impl::getTypeFromMode(int mode) const
{
    auto tm = getTypeMap();
    auto it = tm.find(mode);
    return it != tm.end() ? it->second : typeNull;
} // function ImageIO::Impl.getTypeFromMode

int ImageIO::Impl::getModeFromType(const Type &type) const
{
    for (auto &pair: getTypeMap())
        if (type == pair.second)
            return pair.first;

    return -999;
} // function ImageIO::Impl.getTypeFromMode


void ImageIO::Impl::toStream(std::ostream &ostream, int verbosity) const
{

}

size_t ImageIO::fread(FILE *file, void *data, size_t count,
                      size_t typeSize, bool swap)
{
    size_t  out = ::fread(data, count, typeSize, file);

    if (swap)
        Type::swapBytes(data, count, typeSize);
    return out;
} // function ImageIO::fread

size_t ImageIO::fread(FILE *file, Array &array, bool swap)
{
    return fread(file, array.getData(), array.getDim().getSize(),
                 array.getType().getSize(), swap);
} // function ImageIO::fread

#include "image_formats/image_mrc.cpp"
#include "image_formats/image_spider.cpp"
#include "image_formats/image_tiff.cpp"
#include "image_formats/image_em.cpp"
#include "image_formats/image_dm.cpp"
#include "image_formats/image_imagic.cpp"

