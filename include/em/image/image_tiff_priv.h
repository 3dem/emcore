//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_TIFF_H
#define EM_CORE_RW_TIFF_H

#include "em/image/image.h"

using namespace em;

class ImageTiffIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual std::string getExtensions() const override;

    //virtual void read(const ImageLocation &location, Image &image) override;

    //virtual void openFile(const std::string &path) override;
    virtual void read(const size_t index, Image &image) override;
    //virtual void closeFile() override;

    virtual ~ImageTiffIO();

protected:
    virtual void readHeader() override ;
    virtual ImageIO* create() const override;
}; // class ImageTiffIO

#endif // EM_CORE_RW_TIFF_H