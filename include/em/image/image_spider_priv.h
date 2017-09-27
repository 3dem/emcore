//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_SPIDER_H
#define EM_CORE_RW_SPIDER_H

#include "em/image/image.h"

using namespace em;

class ImageSpiderIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual StringVector getExtensions() const override;

    virtual ~ImageSpiderIO();

protected:
    virtual ImageHandler* createHandler() override ;
    virtual void readHeader() override ;
    virtual size_t getHeaderSize() const override ;
    virtual ImageIO* create() const override ;

}; // class ImageSpiderIO

#endif // EM_CORE_RW_SPIDER_H