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
    ImageFile imgio;
    imgio.open(location.path);
    // FIXME: Now only reading the first image in the location range
    imgio.read(location.index, *this);
    imgio.close();
} // function Image::read

void Image::write(const ImageLocation &location) const
{
    ImageFile imgio;

    if (Path::exists(location.path))
        imgio.open(location.path,  File::Mode::READ_WRITE);
    else
    {
        imgio.open(location.path,  File::Mode::TRUNCATE);
        imgio.createEmpty(getDim(), getType());
    }

    imgio.write(location.index, *this);
    imgio.close();
} // function Image::write


// ===================== ImageFile Implementation =======================

using ImageIOImplRegistry = ImplRegistry<ImageFile::Impl>;


/** Helper function to sort a given vector of types. */
void sortTypeVector(TypeVector &typesVector)
{
    std::sort(typesVector.begin(), typesVector.end(),
              [](const Type& t1, const Type& t2)
              {
                  if (t1.getSize() == t2.getSize())
                  {
                      return t1.getName() < t2.getName();
                  }
                  else return t1.getSize() < t2.getSize();
              });
} // function sortTypeVector

ImageIOImplRegistry * getImageIORegistry()
{
    static ImageIOImplRegistry registry;
    return &registry;
} // function getImageIORegistry

bool em::ImageFile::registerImpl(const StringVector &extOrNames,
                             ImageFile::ImplBuilder builder)
{
    return getImageIORegistry()->registerImpl(extOrNames, builder);
} // function registerImageIOImpl

bool ImageFile::hasImpl(const std::string &extension)
{
    return getImageIORegistry()->hasImpl(extension);
} // function hasIO

std::vector<Type> ImageFile::getImplTypes(const std::string &extOrName)
{
    auto impl = getImageIORegistry()->buildImpl(extOrName);
    auto types = impl->getTypes();
    delete impl;

    return types;
} // function ImageFile::getImplTypes

ImageFile::FormatTypes ImageFile::getFormatTypes()
{
    FormatTypes dict;

    for (const auto& kv: getImageIORegistry()->getUniqueMap())
    {
        auto& vector = dict[kv.first] = {};  // empty vector
        auto impl = kv.second();
        for (const auto& kv2: impl->getTypeMap())
            vector.push_back(kv2.second);
        sortTypeVector(vector);
    }
    return dict;
} // ImageFile::getFormatTypes

size_t ImageFile::fread(FILE *file, void *data, size_t count,
                        size_t typeSize, bool swap)
{
    size_t  out = ::fread(data, count, typeSize, file);

    if (swap)
        Type::swapBytes(data, count, typeSize);
    return out;
} // function ImageFile::fread

size_t ImageFile::fread(FILE *file, Array &array, bool swap)
{
    return fread(file, array.getData(), array.getDim().getSize(),
                 array.getType().getSize(), swap);
} // function ImageFile::fread


ImageFile::ImageFile()
{
    impl = nullptr;
} // Empty Ctor ImageFile

ImageFile::ImageFile(const std::string &path, const em::File::Mode mode,
                     const std::string &formatName):ImageFile()
{
    open(path, mode, formatName);
} // Ctor ImageFile

void ImageFile::open(const std::string &path, const em::File::Mode mode,
                     const std::string &formatName)
{
    // Users should call ImageFile.close method before a new call to open
    ASSERT_ERROR(impl != nullptr, std::string("Already opened file: ") + path);

    auto ext = formatName.empty() ? Path::getExtension(path) : formatName;
    impl = getImageIORegistry()->buildImpl(ext);
    impl->path = path;
    impl->fileMode = mode;

    // If the file does not exists and mode is File::Mode::READ_WRITE
    // switch automatically to TRUNCATE mode
    if (mode ==  File::Mode::READ_WRITE and !Path::exists(path))
        impl->fileMode =  File::Mode::TRUNCATE;

    impl->openFile();

    if (impl->fileMode !=  File::Mode::TRUNCATE)
        impl->readHeader();
} // function ImageFile.open

ArrayDim ImageFile::getDim() const
{
    ASSERT_ERROR(impl == nullptr, "File has not been opened. ");
    return impl->dim;
} // function ImageFile.getDim

Type ImageFile::getType() const
{
    ASSERT_ERROR(impl == nullptr, "File has not been opened. ");
    return impl->type;
} // function ImageFile.getType

// TODO: Allow to read more than one image
// TODO: Allow to read only a slice of a volume
void ImageFile::read(size_t index, Image &image)
{
    // Get first the type of the file, this will check
    // if the file has already been opened
    auto fileType = getType();

    // If type is null, it means that the file was opened
    // in TRUNCATE mode and no image has been written so far,
    // so there is no type and we can't read
    ASSERT_ERROR(fileType.isNull(), "Can not read without a valid type.");

    // Check that the index to be ready is within the file number of images
    ArrayDim adim = getDim(); // This will check that the file was open
    ASSERT_ERROR(index > adim.n, "Invalid index");

    if (index == ImageLocation::ALL)
        index = ImageLocation::FIRST;

    adim.n = 1; // Allocate for just one element for now
    image.resize(adim, fileType);
    impl->readImageData(index, image);

    if (impl->swap)
        Type::swapBytes(image.getData(), adim.getItemSize(),
                        fileType.getSize());
} // function ImageFile::read

void ImageFile::write(size_t index, const Image &image)
{
    // Get first the type of the file, this will check
    // if the file has already been opened
    auto fileType = getType();
    auto imageType = image.getType();

    // If the file has been opened with TRUNCATE, it will
    // not have any type and we will use the one from the image
    // Otherwise, the image and file type should be the same
    if (fileType.isNull())
    {
        auto adim = image.getDim();
        adim.n = index;
        createEmpty(adim, imageType);
    }
    else
        ASSERT_ERROR(imageType != fileType,
                 "Image should have the same type of the file.");

    // FIXME: Check what to do with ALL as index
    if (index == ImageLocation::ALL)
        index = ImageLocation::FIRST;

    // If the file is not big enough to write in this position,
    // let's expand it to enable the write operation
    expand(index);

    impl->writeImageData(index, image);
} // function ImageFile::write

void ImageFile::createEmpty(const ArrayDim &adim, const Type & type)
{
    // Input type can not be null to create a file
    ASSERT_ERROR(type.isNull(), "Input type can not be null. ");

    // Create empty file only for TRUNCATE mode
    ASSERT_ERROR(impl->fileMode !=  File::Mode::TRUNCATE,
                 "ImageFile::createEmpty can only be used with TRUNCATE mode.");

    // Check that requested type is supported for the format implementation
    auto types = impl->getTypes();
    ASSERT_ERROR(std::find(types.begin(), types.end(), type) == types.end(),
                 std::string("Unsupported type '") + type.getName() +
                 std::string("' for this file format: ") + impl->path);

    // Set new type and dimensions
    impl->dim = adim;
    impl->type = type;
    impl->writeHeader(); // write the main header of the file
    impl->expand();
} // function ImageFile.createEmpty

void ImageFile::expand(const size_t ndim)
{
    // Get first the type of the file, this will check
    // if the file has already been opened
    auto fileType = getType();

    ASSERT_ERROR(impl->fileMode == File::READ_ONLY,
                 std::string("Can not expand a file opened as READ_ONLY: ")
                 + impl->path);

    // Check the type has been set before calling expand
    // we can arrive here via createEmpty file, where type should be set
    // or from write, where also the type can be inferred from image
    ASSERT_ERROR(fileType.isNull(),
                 "Can not expand file without a valid type.");

    // After all validations, call the implementation expand method
    if (ndim > impl->dim.n)
    {
        impl->dim.n = ndim;
        impl->expand();
    }
} // function expand


void ImageFile::toStream(std::ostream &ostream, int verbosity) const
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

std::ostream& em::operator<< (std::ostream &ostream, const em::ImageFile &imageIO)
{
    imageIO.toStream(ostream);
    return ostream;
}

void ImageFile::close()
{
    if (impl != nullptr)
    {
        impl->closeFile();
        delete impl;
        impl = nullptr;
    }
} // function ImageFile.close

ImageFile::~ImageFile()
{
    close();
}// ImageFile ctor


// ===================== ImageFile::Impl Implementation =======================
ImageFile::Impl::~Impl()
{
}

TypeVector ImageFile::Impl::getTypes() const
{
    TypeVector types;

    for (const auto& kv2: getTypeMap())
        types.push_back(kv2.second);

    sortTypeVector(types);

    return types;
} // function ImageFile::Impl.getTypes

size_t ImageFile::Impl::getHeaderSize() const
{
    return 0;
} // function ImageFile::Impl::getHeaderSize

size_t ImageFile::Impl::getPadSize() const
{
    return pad;
} // function ImageFile::Impl::getPadSize

size_t ImageFile::Impl::getImageSize() const
{
    return dim.getItemSize() * type.getSize() + getPadSize();
} // function ImageFile::Impl::getImageSize

const char * ImageFile::Impl::getModeString() const
{
    return File::modeToString(fileMode);
} // function ImageFile::Impl::getModeString

void ImageFile::Impl::openFile()
{
    file = fopen(path.c_str(), getModeString());

    if (file == nullptr)
        THROW_SYS_ERROR(std::string("Error opening file: ") + path);
} // function ImageFile::Impl::openFile

void ImageFile::Impl::closeFile()
{
    if (file != nullptr)
    {
        fclose(file);
        file = nullptr;
    }
} // function ImageFile::Impl::closeFile

void ImageFile::Impl::expand()
{
    // Compute the size of one item, taking into account its x, y, z dimensions
    // and the size of the type that will be used
    size_t itemSize = getImageSize();

    // Compute the total size of the file taking into account the general header
    // size and the size of all items (including extra padding per item)
    size_t fileSize = getHeaderSize() + itemSize * dim.n;

    File::resize(file, fileSize);
    fflush(file);
} // function ImageFile::Impl::expand

void ImageFile::Impl::readImageHeader(const size_t index, Image &image) {}

void ImageFile::Impl::writeImageHeader(const size_t index, const Image &image) {}

void ImageFile::Impl::readImageData(const size_t index, Image &image)
{
    size_t itemSize = getImageSize(); // Size of an item containing the padSize
    size_t padSize = getPadSize();
    size_t readSize = itemSize - padSize;
    // Compute the position of the item data in the file given its size
    size_t itemPos = getHeaderSize() + itemSize * (index - 1) + padSize;

    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR(std::string("Could not 'fseek' in file. ") + path);

    // FIXME: change this to read by chunks when we change this
    // approach, right now only read a big chunk of one item size

    if (::fread(image.getData(), readSize, 1, file) != 1)
        THROW_SYS_ERROR(std::string("Could not 'fread' data from file: ") + path);
}

void ImageFile::Impl::writeImageData(const size_t index, const Image &image)
{
    size_t itemSize = getImageSize();
    size_t padSize = getPadSize();
    size_t writeSize = itemSize - padSize;
    size_t itemPos = getHeaderSize() + itemSize * (index - 1) + padSize;

    //std::cerr << "ImageFile::Impl::write: itemPos: " << itemPos << std::endl;
    //std::cerr << "ImageFile::Impl::write: itemSize: " << itemSize << std::endl;

    if (fseek(file, itemPos, SEEK_SET) != 0)
        THROW_SYS_ERROR(std::string("Could not 'fseek' in file: ") + path);

    fwrite(image.getData(), writeSize, 1, file);

} // function ImageFile::Impl::write

Type ImageFile::Impl::getTypeFromMode(int mode) const
{
    auto tm = getTypeMap();
    auto it = tm.find(mode);
    return it != tm.end() ? it->second : typeNull;
} // function ImageFile::Impl.getTypeFromMode

int ImageFile::Impl::getModeFromType(const Type &type) const
{
    for (auto &pair: getTypeMap())
        if (type == pair.second)
            return pair.first;

    return -999;
} // function ImageFile::Impl.getTypeFromMode


void ImageFile::Impl::toStream(std::ostream &ostream, int verbosity) const
{

}



#include "image_formats/image_mrc.cpp"
#include "image_formats/image_spider.cpp"
#include "image_formats/image_tiff.cpp"
#include "image_formats/image_em.cpp"
#include "image_formats/image_dm.cpp"
#include "image_formats/image_imagic.cpp"
#include "image_formats/image_png.cpp"
#include "image_formats/image_jpeg.cpp"

