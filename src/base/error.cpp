//
// Created by josem on 7/13/17.
//

#include "em/base/error.h"
#include <cstring>

using namespace em;


Error::Error(const std::string &msg, const std::string &filename, const long line, const int errorCode)
{
    this->msg = msg;
    this->filename = filename;
    this->line = line;
    this->errorCode = errorCode;
}

std::ostream& em::operator<< (std::ostream &ostream, const Error &err)
{
    ostream << "ERROR: " << err.msg << std::endl;

    if (err.errorCode != 0)
    {
        ostream << "   Code: " << err.errorCode
                << " (" << strerror(err.errorCode) << ")" << std::endl;
    }

    ostream << "   File: " << err.filename
            << "  (Line " << err.line << ")" << std::endl;

    return ostream;
}