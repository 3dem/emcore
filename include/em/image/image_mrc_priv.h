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
    virtual StringVector getExtensions() const override;
    virtual ~ImageMrcIO();

protected:
    virtual ImageHandler* createHandler() override ;
    virtual void readHeader() override ;
    virtual size_t getHeaderSize() const override ;
    virtual ImageIO* create() const override;
}; // class ImageMrcIO

#endif // EM_CORE_RW_MRC_H