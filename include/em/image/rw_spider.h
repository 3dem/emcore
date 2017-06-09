//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_SPIDER_H
#define EM_CORE_RW_SPIDER_H

#include <iostream>
#include <sstream>
#include <vector>

#include "em/image/image.h"

using namespace em;

class SpiderReader: public ImageReader
{

public:
    virtual std::string getName() const;
    virtual std::string getExtensions() const;

    virtual void read(const ImageLocation &location, Image &image) override;

    virtual void openFile(const std::string &path) override;
    virtual void read(const size_t index, Image &image) override;
    virtual void closeFile();

private:
    virtual ImageReader* create() const;
}; // class SpiderReader

#endif // EM_CORE_RW_SPIDER_H