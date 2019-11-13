//
// Created by josem on 11/7/17.
//

#include "emc/base/string.h"
#include "emc/proc/processor.h"
#include "../../include/emc/proc/fft.h"

using namespace emcore;

ImageProcessor::ImageProcessor(const ObjectDict &params)
{
    setParams(params);
}

void ImageProcessor::setParams(const ObjectDict &params)
{
    this->params = params;
    validateParams();
} // ImageProcessor ctor

bool ImageProcessor::hasParam(const std::string &paramName) const
{
    return params.find(paramName) != params.end();
}

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

    for (auto& proc: processors)
    {
        proc->process(localInput, output);
        localInput = output;
    }
} // function ImagePipeProc.process

void ImagePipeProc::process(Image &inputOutput)
{
    for (auto& proc: processors)
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


// -------------- ImageScaleProc Implementation ---------------------------
void ImageScaleProc::validateParams() const
{
    int count = 0;

    for (auto& param: {"newdim_x", "newdim_y", "factor", "angpix_old"})
        if (hasParam(param))
            count++;

    ASSERT_ERROR(count == 0, "Please provide at least one of the valid parameter.");
    ASSERT_ERROR(count > 1, "Please provide only non-exclusive parameters. ");
    if (hasParam("angpix_old"))
        ASSERT_ERROR(!hasParam("angpix_new"),
                     "Please provide angpix_new when using angpix_old. ");
}


void ImageScaleProc::process(const Image &input, Image &output)
{
    FourierTransformer ft;
    // TODO: Check if we need to convert always
    auto inputDim = input.getDim();

    float scaleFactor;

    if (hasParam("factor"))
        scaleFactor = params["factor"].get<float>();
    else if (hasParam("newdim_x"))
        scaleFactor = params["newdim_x"].get<float>() / inputDim.x;
    else if (hasParam("newdim_y"))
        scaleFactor = params["newdim_y"].get<float>() / inputDim.y;
    else if (hasParam("angpix_old"))
        scaleFactor = params["angpix_old"].get<float>() / params["angpix_new"].get<float>();
    else
        THROW_ERROR("Invalid parameters.");

    auto maxdim = std::max(inputDim.x, inputDim.y);
    auto scaleMaxdim = maxdim * scaleFactor;
    // Create a float and square image to perform the FT
    output.resize(ArrayDim(maxdim, maxdim), typeFloat);
    output.patch(input);  // Copy input into the squared image

    Image tmp;
    ft.scale(output, tmp, scaleMaxdim);
    output.resize(ArrayDim(inputDim.x * scaleFactor, inputDim.y * scaleFactor));
    output.extract(tmp);

} // function ImageScaleProc.process

void ImageScaleProc::process(Image &image)
{
    Image tmp;
    process(image, tmp);
    std::swap(image, tmp);  // Move the result to image
} // function ImageScaleProc.process


// -------------- ImageWindowProc Implementation ---------------------------
void ImageWindowProc::validateParams() const
{
    int count = 0;

//    for (auto& param: {"newdim_x", "newdim_y", "factor", "angpix_old"})
//        if (hasParam(param))
//            count++;
//
//    ASSERT_ERROR(count == 0, "Please provide at least one of the valid parameter.");
//    ASSERT_ERROR(count > 1, "Please provide only non-exclusive parameters. ");
//    if (hasParam("angpix_old"))
//        ASSERT_ERROR(!hasParam("angpix_new"),
//                     "Please provide angpix_new when using angpix_old. ");
}

void ImageWindowProc::process(const Image &input, Image &output)
{
    FourierTransformer ft;
    // TODO: Check if we need to convert always
    auto inputDim = input.getDim();

    std::cout << "Croping image with dims: " << inputDim << std::endl;
    output = input;
} // function ImageWindowProc.process

void ImageWindowProc::process(Image &image)
{
    Image tmp;
    process(image, tmp);
    std::swap(image, tmp);  // Move the result to image
} // function ImageWindowProc.processIm