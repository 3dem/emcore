#include <iostream>
#include <sstream>
#include <vector>

#include "em/image/rw_spider.h"


std::string SpiderReader::getName() const
{
    return "spider";
}

std::string SpiderReader::getExtensions() const
{
    return "spi";
}

void SpiderReader::read(const em::ImageLocation &location, em::Image &image)
{

}

void SpiderReader::openFile(const std::string &path)
{

}

void SpiderReader::read(const size_t index, em::Image &image)
{

}

void SpiderReader::closeFile()
{

}

em::ImageReader* SpiderReader::create() const
{
    return new SpiderReader();
}
