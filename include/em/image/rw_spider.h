//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_SPIDER_H
#define EM_CORE_RW_SPIDER_H

#include "em/image/image.h"

using namespace em;

class SpiderIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual std::string getExtensions() const override;

    virtual void read(const ImageLocation &location, Image &image) override;

    virtual void openFile(const std::string &path) override;
    virtual void read(const size_t index, Image &image) override;
    virtual void closeFile() override;

private:
    virtual ImageIO* create() const override;

    FILE* imageFile = NULL;
}; // class SpiderIO

#endif // EM_CORE_RW_SPIDER_H