#include <cstdio>

#include "em/image/rw_mrc.h"


std::string MrcIO::getName() const
{
    return "mrc";
}

std::string MrcIO::getExtensions() const
{
    return "mrc";
}

void MrcIO::read(const em::ImageLocation &location, em::Image &image)
{

}

void MrcIO::openFile(const std::string &path)
{

    FILE* imageFile = fopen(path.c_str(), "rb");
    data["imageFile"] = Object(imageFile);
}

void MrcIO::read(const size_t index, em::Image &image)
{

}

void MrcIO::closeFile()
{
    FILE* imageFile = static_cast<FILE*>(data["imageFile"]);
    fclose(imageFile);
}

MrcIO::~MrcIO()
{

}

em::ImageIO* MrcIO::create() const
{
    return new MrcIO();
}
