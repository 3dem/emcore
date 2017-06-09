#include <iostream>
#include <sstream>
#include <vector>

#include "em/image/rw_mrc.h"


std::string MrcReader::getName() const
{
    return "mrc";
}

std::string MrcReader::getExtensions() const
{
    return "mrc";
}

void MrcReader::read(const em::ImageLocation &location, em::Image &image)
{

}

void MrcReader::openFile(const std::string &path)
{

}

void MrcReader::read(const size_t index, em::Image &image)
{

}

void MrcReader::closeFile()
{

}

em::ImageReader* MrcReader::create() const
{
    return new MrcReader();
}
