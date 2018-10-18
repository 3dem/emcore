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
    Type::Operation op = (*this)[OPERATION].get<Type::Operation>();
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


// -------------- Stats Implementation ---------------------------

void Stats::compute(const Array &array)
{
    //FIXME: Now using a copy of the array in float to simplify code
    // this could be implemented at Type level and save the extra copy
    // that is being used now
    Array arrayFloat;
    arrayFloat.copy(array, typeFloat);

    auto dim = array.getDim();
    auto const data = static_cast<const float *>(arrayFloat.getData());
    auto size = dim.getSize();

    min = max = data[0];
    std = mean = 0;

    for (size_t i = 0; i < size; ++i)
    {
        double v = data[i];
        if (v < min)
            min = v;
        else if (v > max)
            max = v;
        mean += v;
        std += v * v;
    }

    if (size > 1)
    {
        mean /= size;
        std = std / size - mean * mean;
        std *= size / (size - 1);

        // Foreseeing numerical instabilities
        std = sqrt(static_cast< double >(abs(std)));
    }
    else
        std = 0;
} // Stats.compute

std::ostream& em::operator<< (std::ostream &ostrm, const Stats &s)
{
    ostrm << "min: " << s.min << " max: " << s.max
          << " avg: " << s.mean << " std: " << s.std << " ";
    return ostrm;
}