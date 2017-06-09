//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_MRC_H
#define EM_CORE_RW_MRC_H

#include "em/image/image.h"

using namespace em;

class MrcReader: public ImageReader
{

public:
    virtual std::string getName() const;
    virtual std::string getExtensions() const;

    virtual void read(const ImageLocation &location, Image &image) override;

    virtual void openFile(const std::string &path) override;
    virtual void read(const size_t index, Image &image) override;
    virtual void closeFile() override;

private:
    virtual ImageReader* create() const;
}; // class MrcReader

#endif // EM_CORE_RW_MRC_H