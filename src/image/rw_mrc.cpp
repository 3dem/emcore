#include <iostream>
#include <sstream>
#include <vector>

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

}

void MrcIO::read(const size_t index, em::Image &image)
{

}

void MrcIO::closeFile()
{

}

em::ImageIO* MrcIO::create() const
{
    return new MrcIO();
}
