//
// Created by josem on 2017-06-09.
//

#ifndef EM_CORE_RW_MRC_H
#define EM_CORE_RW_MRC_H

#include "em/image/image.h"

using namespace em;

class MrcIO: public ImageIO
{

public:
    virtual std::string getName() const override;
    virtual std::string getExtensions() const override;

    virtual void read(const ImageLocation &location, Image &image) override;

    virtual void openFile(const std::string &path) override;
    virtual void read(const size_t index, Image &image) override;
    virtual void closeFile() override;

    virtual ~MrcIO();

protected:
    virtual ImageIO* create() const override;
}; // class MrcIO

#endif // EM_CORE_RW_MRC_H