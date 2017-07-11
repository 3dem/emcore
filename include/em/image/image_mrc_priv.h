//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_MRC_H
#define EM_CORE_RW_MRC_H

#include "em/image/image.h"

using namespace em;

class ImageMrcIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual std::string getExtensions() const override;

    virtual void read(const size_t index, Image &image) override;
    virtual ~ImageMrcIO();

protected:
    virtual ImageHandler* createHandler() override ;
    virtual void readHeader() override ;
    virtual ImageIO* create() const override;
}; // class ImageMrcIO

#endif // EM_CORE_RW_MRC_H