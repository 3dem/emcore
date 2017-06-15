//
// Created by josem on 2017-06-09.
//

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

#include "em/base/type.h"
#include "em/base/array.h"
#include "em/image/image.h"



using namespace em;


// ===================== ImageImpl Implementation =======================

class ImageImpl
{
public:
    // headers[0] will be the main header and
    // the rest one per image
    std::vector<ObjectDict> headers;
    static std::map<std::string, const ImageIO*> readers;
    //static std::map<std::string, ImageWriter*> writers;

    ImageImpl()
    {
        // Create at least one map to store the header of the main image
        headers.push_back(ObjectDict());
    }
};

std::map<std::string, const ImageIO*> ImageImpl::readers;


// ===================== Image Implementation =======================

Image::Image(): Array()
{
    implPtr = new ImageImpl();
} // empty Ctor

Image::Image(const ArrayDim &adim, ConstTypePtr type): Array(adim, type)
{
    implPtr = new ImageImpl();
    // Type should be not null (another option could be assume float or double by default)
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
    ostream << "Header: " << std::endl;
    for (auto& x: implPtr->headers[0]) {
        std::cout << x.first << ": " << x.second << std::endl;
    }
    Array::toStream(ostream);
}

std::ostream& em::operator<< (std::ostream &ostream, const em::Image &image)
{
    image.toStream(ostream);
    return ostream;
}

bool Image::registerIO(const ImageIO *reader)
{
    ImageImpl::readers[reader->getExtensions()] = reader;
    ImageImpl::readers[reader->getName()] = reader;
    return true;
} // function registerIO

bool Image::hasIO(const std::string &extension)
{
    auto it = ImageImpl::readers.find(extension);
    return it != ImageImpl::readers.end();
} // function hasIO

ImageIO* Image::getIO(const std::string &extension)
{
    if (!Image::hasIO(extension))
        return nullptr;

    return ImageImpl::readers[extension]->create();
} // function getIO


// ===================== ImageIO Implementation =======================

#include "em/image/rw_spider.h"

REGISTER_IMAGE_IO(SpiderIO);

#include "em/image/rw_mrc.h"

REGISTER_IMAGE_IO(MrcIO);