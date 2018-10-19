//
// Created by josem on 11/7/17.
//

#include "em/base/string.h"
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

template <typename T>
void computeStats(Stats& s, const Array& array)
{
    auto& type = Type::get<T>();

    if (array.getType() != type)
        THROW_ERROR("Mismatch types of the array and the template.");

    auto dim = array.getDim();
    // Cast away const-ness, we promess not to modify the array content ;)
    Array * arrayPtr = const_cast<Array*>(&array);
    auto const data = arrayPtr->getView<T>().getData();
    auto size = dim.getSize();

    s.min = s.max = data[0];
    s.std = s.mean = 0;

    for (size_t i = 0; i < size; ++i)
    {
        double v = data[i];
        if (v < s.min)
            s.min = v;
        else if (v > s.max)
            s.max = v;
        s.mean += v;
        s.std += v * v;
    }

    if (size > 1)
    {
        s.mean /= size;
        s.std = s.std / size - s.mean * s.mean;
        s.std *= size / (size - 1);

        // Foreseeing numerical instabilities
        s.std = sqrt(static_cast< double >(abs(s.std)));
    }
    else
        s.std = 0;
}

//template <typename T, typename... Args>
//void computeStats(Stats& s, const Array& array)
//{
//    return computeStats<T>(s, array) if (array.getType() == Type::get<T>())
//};


void Stats::compute(const Array &array)
{
    auto& arrayType = array.getType();

#define OPERATE_IF_TYPE(type) if (arrayType == Type::get<type>()) return computeStats<type>(*this, array)
    OPERATE_IF_TYPE(float);
    OPERATE_IF_TYPE(double);
    OPERATE_IF_TYPE(int8_t);
    OPERATE_IF_TYPE(uint8_t);
    OPERATE_IF_TYPE(int16_t);
    OPERATE_IF_TYPE(uint16_t);
    OPERATE_IF_TYPE(int32_t);
    OPERATE_IF_TYPE(uint32_t);
    OPERATE_IF_TYPE(int64_t);
    OPERATE_IF_TYPE(uint64_t);
    THROW_ERROR(std::string("Operate has not been implemented for type: ") + arrayType.getName());
#undef OPERATE_IF_TYPE
} // Stats.compute

std::ostream& em::operator<< (std::ostream &ostrm, const Stats &s)
{
    ostrm << "min: " << s.min << " max: " << s.max
          << " avg: " << s.mean << " std: " << s.std << " ";
    return ostrm;
}