//
// Created by josem on 11/7/17.
//

#include "em/proc/processor.h"

using namespace em;


Object& ImageProcessor::operator[](const std::string &key)
{
    return params[key];
}

const Object& ImageProcessor::operator[](const std::string &key) const
{
    return params.at(key);
}