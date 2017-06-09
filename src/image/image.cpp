//
// Created by josem on 1/7/17.
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

    ImageImpl()
    {
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
    ostream << "Image: " << std::endl;
    Array::toStream(ostream);
}

std::ostream& em::operator<< (std::ostream &ostream, const em::Image &image)
{
    image.toStream(ostream);
    return ostream;
}