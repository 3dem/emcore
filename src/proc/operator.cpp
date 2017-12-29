//
// Created by josem on 11/7/17.
//

#include "em/proc/operator.h"

using namespace em;

const std::string ImageMathProc::OPERATION = "operation";
const std::string ImageMathProc::OPERAND = "operand";


template <class T>
void processImage(Image &image, const Object &object, ImageMathProc::Operation op)
{
    ArrayDim adim = image.getDim();
    size_t size = adim.getSize(), i = 0;

    T * data = static_cast<T*>(image.getPointer());

    if (object.getType() == Type::get<Image>())
    {
        Image image2 = (Image) object;
        T * data2 = static_cast<T*>(image2.getPointer());

        switch (op)
        {
            case ImageMathProc::ADD:
                for (; i < size; ++data, ++data2, ++i)
                    *data += *data2;
                break;
            case ImageMathProc::SUB:
                for (; i < size; ++data, ++data2, ++i)
                    *data -= *data2;
                break;
            case ImageMathProc::MUL:
                for (; i < size; ++data, ++data2, ++i)
                    *data *= *data2;
                break;
            case ImageMathProc::DIV:
                for (; i < size; ++data, ++data2, ++i)
                    *data /= *data2;
                break;
        } // switch (op)
    }
    else
    {
        T value = (T) object;
        switch (op)
        {
            case ImageMathProc::ADD:
                for (; i < size; ++data, ++i)
                    *data += value;
                break;
            case ImageMathProc::SUB:
                for (; i < size; ++data, ++i)
                    *data -= value;
                break;
            case ImageMathProc::MUL:
                for (; i < size; ++data, ++i)
                    *data *= value;
                break;
            case ImageMathProc::DIV:
                for (; i < size; ++data, ++i)
                    *data /= value;
                break;
        } // switch (op)
    }
} // function processImage<T>


ImageMathProc::ImageMathProc()
{
#define REGISTER_TYPE(type) map[Type::get<type>()] = processImage<type>;

    REGISTER_TYPE(float);
    REGISTER_TYPE(uint8_t);
    REGISTER_TYPE(uint16_t);
}

void ImageMathProc::process(const Image &input, Image &output)
{
    output = input;
    process(output);
}

void ImageMathProc::process(Image &image)
{
    ProcessFunc f = map[image.getType()];

    if (f != nullptr)
        f(image, (*this)[OPERAND], (*this)[OPERATION]); return;

    THROW_ERROR("Unsupported type.");
}

