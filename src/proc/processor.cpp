//
// Created by josem on 11/7/17.
//

#include "em/base/string.h"
#include "em/proc/processor.h"
#include "../../include/em/proc/fft.h"

using namespace em;

void ImageProcessor::setParams(
        std::initializer_list<std::pair<std::string, Object>> list)
{
    for (auto& pair: list)
    {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
        (*this)[pair.first] = pair.second;
    }

    validateParams();
} // ImageProcessor ctor

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

size_t ImagePipeProc::getSize() const
{
    return processors.size();
} // function ImagePipeProc.getSize

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
        proc->process(inputOutput);
} // function ImagePipeProc.process


// -------------- ImageMathProc Implementation ---------------------------

const std::string ImageMathProc::OPERATION = "operation";
const std::string ImageMathProc::OPERAND = "operand";


void ImageMathProc::process(const Image &input, Image &output)
{
    output = input;
    process(output);
}

void ImageMathProc::process(Image &image)
{
    // Just initialize with the proper type
    Type::Operation op = params[OPERATION].get<Type::Operation>();
    auto &operand = params[OPERAND];

    switch (op)
    {
        case Type::ADD:
            image += operand;
            break;
        case Type::SUB:
            image -= operand;
            break;
        case Type::MUL:
            image *= operand;
            break;
        case Type::DIV:
            image /= operand;
            break;
        default:
            THROW_ERROR("Unsupported operation.");
    } // switch
} // function ImageMathProc.process


// -------------- ImageMathProc Implementation ---------------------------

void ImageScaleProc::process(const Image &input, Image &output)
{
    FourierTransformer ft;
    auto newdim = params["newdim"].get<int>();

    // Check if we need to convert always
    Image tmp;
    tmp.copy(input, typeFloat);

    ft.scale(tmp, output, newdim);
} // function ImageScaleProc.process

void ImageScaleProc::process(Image &image)
{
    Image tmp;
    process(image, tmp);
    std::swap(image, tmp);  // Move the result to image
} // function ImageScaleProc.process

