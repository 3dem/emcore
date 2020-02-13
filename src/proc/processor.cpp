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

void ImageProcessor::printParams() const
{
    std::cout << "Params: " << std::endl ;
    for (const auto& kv : this->params)
    {
        std::cout << "   " << kv.first << ": " << kv.second << std::endl;
    }
}

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

const std::string ImageProcessor::OPERATION = "operation";
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
void ImageScaleProc::validateParams()
{
    int count = 0;

    for (auto& param: {"newdim_x", "newdim_y", "factor", "angpix_old",
                       "scale_arg"})
        if (hasParam(param))
            count++;

    ASSERT_ERROR(count == 0, "Please provide at least one of the valid parameter.");
    ASSERT_ERROR(count > 1, "Please provide only non-exclusive parameters. ");
    if (hasParam("angpix_old"))
        ASSERT_ERROR(!hasParam("angpix_new"),
                     "Please provide angpix_new when using angpix_old. ");

    if (hasParam("angpix_old"))
        params["factor"] = params["angpix_old"].get<float>() / params["angpix_new"].get<float>();
    // TODO: Allow more options for scaling, for now only scale factor
    else if (hasParam("scale_arg"))
        params["factor"] = String::toFloat(params["scale_arg"].toString().c_str());
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
void ImageWindowProc::validateParams()
{
    int count = 0;

    if (hasParam("crop_values"))
    {
        // TODO: Parse other options of crop values to allow to specify
        // other options
        auto cropValues = params["crop_values"].toString();

        try
        {
            auto parts = String::split(cropValues, ',');
            auto n = parts.size();
            int left = String::toInt(parts[0]);

            params["_left"] = left;
            params["_top"] = n > 1 ? String::toInt(parts[1]) : params["_left"];
            params["_right"] = n > 2 ? String::toInt(parts[2]) : params["_left"];
            params["_bottom"] = n > 3 ? String::toInt(parts[3]) : params["_top"];
        }
        catch (const Error &e)
        {
            THROW_ERROR(std::string("The following error occurred parsing "
                                    "'crop' arguments: " + cropValues + "\n" +
                                    e.toString()));
        }
    }
    else if (hasParam("window_p1") && hasParam("window_p2"))
    {
        printParams();
    }
    else
        THROW_ERROR("Please provide either 'crop_values' or "
                    "'window_p1' and 'window_p2'");
} // function ImageWindowProc.validateParams

std::vector<int> parsePointString(const std::string &pointStr)
{
    auto parts = String::split(pointStr, ',');
    ASSERT_ERROR(parts.size() > 2,
                 "Only 2D points are implemented so far.")

    std::vector<int> result;
    for (auto const &p: parts)
        result.push_back(String::toInt(p));

    return result;
}

void ImageWindowProc::process(const Image &input, Image &output)
{
    FourierTransformer ft;
    // TODO: Check if we need to convert always
    auto inputDim = input.getDim();
    auto inputType = input.getType();
    auto xDim = (int) inputDim.x;
    auto yDim = (int) inputDim.y;

    ASSERT_ERROR(inputDim.z > 1, "Crop/Window not implemented for volumes yet.")

    if (hasParam("crop_values"))
    {
        auto x1 = params["_left"].get<int>();
        auto y1 = params["_bottom"].get<int>();
        auto y2 = yDim - params["_top"].get<int>();
        auto x2 = xDim - params["_right"].get<int>();

        auto newDim = ArrayDim(x2 - x1, y2 - y1, 1);
        output.resize(newDim, inputType);
        output.extract(input, x1, y1, 0);
    }
    else  // window operation
    {
        auto p1 = parsePointString(params["window_p1"].toString());
        auto p2 = parsePointString(params["window_p2"].toString());

        float fillValue = 0.0;

        if (hasParam("window_fill"))
            fillValue = String::toFloat(params["window_fill"].toString());

        auto x1 = std::min(p1[0], p2[0]);
        auto y1 = std::min(p1[1], p2[1]);
        auto x2 = std::max(p1[0], p2[0]);
        auto y2 = std::max(p1[1], p2[1]);

        ASSERT_ERROR(x1 > xDim || x2 < 0,
                     "Non-overlapping X range for windowing.")

        ASSERT_ERROR(y1 > yDim || y2 < 0,
                     "Non-overlapping Y range for windowing.")

        auto newDim = ArrayDim(x2 - x1, y2 - y1, 1);
        output.resize(newDim, inputType);

        //std::cout << "Fill value: " << fillValue << std::endl;

        output.set(fillValue);

        auto x1In = x1 >= 0 && x1 <= xDim;
        auto y1In = y1 >= 0 && y1 <= yDim;
        auto x2In = x2 >= 0 && x2 <= xDim;
        auto y2In = y2 >= 0 && y2 <= yDim;

        if (!(x1In || y1In || x2In || y2In))  // All outside
        {
            //std::cout << "Patching x=" << 0 - x1 << " y=" << 0 - y1 << std::endl;
            output.patch(input, 0 - x1, 0 - y1, 0);
        }
        else
        {
            auto x1o = x1In ? x1 : 0;
            auto y1o = y1In ? y1 : 0;
            auto x2o = x2In ? x2 : xDim;
            auto y2o = y2In ? y2 : yDim;
//            std::cout << "Patching Overlap: "
//                      << " x1o=" << x1o << " y1o=" << y1o
//                      << " x2o=" << x2o << " y2o=" << y2o
//                      << std::endl;
            auto tmpImage = Image(ArrayDim(x2o - x1o, y2o - y1o, 1), inputType);
            tmpImage.extract(input, x1o, y1o, 0);
//            tmpImage.write("tmp.mrc");
//            std::cout << "Patching coords: "
//                      << " x1o - x1=" << x1o - x1 << " y1o - y1=" << y1o - y1
//                      << std::endl;
            output.patch(tmpImage, x1o - x1, y1o - y1, 0);
        }
    }
} // function Command.process

void ImageWindowProc::process(Image &image)
{
    Image tmp;
    process(image, tmp);
    std::swap(image, tmp);  // Move the result to image
} // function ImageWindowProc.process