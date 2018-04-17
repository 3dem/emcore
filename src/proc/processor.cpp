//
// Created by josem on 11/7/17.
//

#include "em/proc/processor.h"

using namespace em;

ImageProcessor::ImageProcessor(
        std::initializer_list<std::pair<std::string, Object>> list)
{
    std::cout << "ImageProcessor, initializing from list:" << std::endl;
    for (auto& pair: list)
    {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
        (*this)[pair.first] = pair.second;
    }
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
    {
        proc->process(inputOutput);
    }
} // function ImagePipeProc.process


// -------------- ImageMathProc Implementation ---------------------------

const std::string ImageMathProc::OPERATION = "operation";
const std::string ImageMathProc::OPERAND = "operand";


void ImageMathProc::process(const Image &input, Image &output)
{
    output.resize(input);
    output = input;
    std::cout << "input: " << input << std::endl;
    std::cout << "output: " << output << std::endl;

    process(output);
}

void ImageMathProc::process(Image &image)
{
    // Just initialize with the proper type
    Type::Operation op = (*this)[OPERATION];
    auto &operand = (*this)[OPERAND];

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