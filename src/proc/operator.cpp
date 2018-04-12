//
// Created by josem on 11/7/17.
//

#include "em/proc/operator.h"

using namespace em;

const std::string ImageMathProc::OPERATION = "operation";
const std::string ImageMathProc::OPERAND = "operand";

ImageMathProc::ImageMathProc()
{
    // Use int to store the value of the enum
    //(*this)[ImageMathProc::OPERATION] = Object(Type::ADD);
}

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

    }
} // function ImageMathProc.process

