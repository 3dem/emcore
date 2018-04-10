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


// -------------- ImagePipeProc Implementation ---------------------------

void ImagePipeProc::addProcessor(ImageProcessor* imgProc)
{
    processors.push_back(imgProc);
} // function ImagePipeProc.addProcessor

void ImagePipeProc::process(const Image &input, Image &output)
{
    Image localInput(input);

    for (auto proc: processors)
    {
        proc->process(localInput, output);
        localInput = output;
    }
} // function ImagePipeProc.process

void ImagePipeProc::process(Image &inputOutput)
{
    for (auto proc: processors)
    {
        proc->process(inputOutput);
    }
} // function ImagePipeProc.process