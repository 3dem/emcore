//
// Created by josem on 7/13/17.
//

#include "em/base/error.h"
#include <cstring>

using namespace em;


Error::Error(const std::string &msg, const std::string &filename, const long line, const int errorCode,
             const std::string &func)
{
    this->msg = msg;
    this->fileName = filename;
    this->line = line;
    this->errorCode = errorCode;
    this->fuctionName = func;
}

std::ostream& em::operator<< (std::ostream &ostream, const Error &err)
{
    ostream << "ERROR: " << err.msg << std::endl;

    if (!err.fuctionName.empty())
        ostream << "   Function: " << err.fuctionName << std::endl;

    if (err.errorCode != 0)
        ostream << "   Code: " << err.errorCode
                << " (" << strerror(err.errorCode) << ")" << std::endl;

    ostream << "   File: " << err.fileName
            << "  (Line " << err.line << ")" << std::endl;

    return ostream;
}