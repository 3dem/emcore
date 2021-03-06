//
// Created by josem on 11/7/17.
//

#ifndef EM_CORE_PROCESSOR_H
#define EM_CORE_PROCESSOR_H

#include "emc/base/image.h"


namespace emcore
{

    /** Base class for all operations that receive an Image as input and produce
     * an Image as output.
     */
    class ImageProcessor
    {
    public:
        /** String constant defintion that can be used if the Processor has
         * more than one operation.
         */
        static const std::string OPERATION;

        /** Default empty constructor. None of the parameters are set. */
        ImageProcessor() = default;
        ImageProcessor(const ObjectDict &params);

        /** Set processor parameters.
         */
        void setParams(const ObjectDict &params);

        /** Print input params (more for debugging)
         */
         void printParams() const;

        bool hasParam(const std::string &paramName) const;

        /** Apply the operation defined by this Processor to the input
         * image and store the result in the output image.
         */
        virtual void process(const Image &input, Image &output) = 0;

        /** Apply the operation defined by this Processor to input image
         * and modify it to store the result.
         * @param inputOutput
         */
        virtual void process(Image &inputOutput) = 0;

        Object & operator[](const std::string &key);
        const Object & operator[](const std::string &key) const;

    protected:
        ObjectDict params;

        /** This method should be overriden to make some validations after
         * the setParams is called.
         */
        virtual void validateParams() {}

    }; // class ImageProcessor


    /** Special type of Processor that holds a list of other processors.
     */
     class ImagePipeProc: public ImageProcessor
     {
     public:
         using ImageProcessor::ImageProcessor;


         virtual void process(const Image &input, Image &output) override ;
         virtual void process(Image &inputOutput) override ;

         /** Add a new processor to the list */
        void addProcessor(ImageProcessor* imgProc);

         /** Return the number of processors */
        size_t getSize() const;

     protected:
         std::vector<ImageProcessor*> processors;
     }; // class ImagePipeProc


    /** Processor to scale images
     */
    class ImageMathProc: public ImageProcessor
    {
    public:

        static const std::string OPERAND;

        using ImageProcessor::ImageProcessor;

        /** Apply the operation defined by this Processor to the input
         * image and store the result in the output image.
         */
        virtual void process(const Image &input, Image &output) override ;

        /** Apply the operation defined by this Processor to input image
         * and modify it to store the result.
         * @param inputOutput
         */
        virtual void process(Image &inputOutput) override ;
    }; // class ImageMathProc


    /** Processor to perform some basic image operations such as:
     * addition, subtraction, multiplication and division
     */
    class ImageScaleProc: public ImageProcessor
    {
        using ImageProcessor::ImageProcessor;

    public:

        /** Scale input image and store the new one in output.
         * The output image will have the dimension defined by param "newdim"
         */
        virtual void process(const Image &input, Image &output) override ;

        /** Apply the scale and store the output in the same input image.
         */
        virtual void process(Image &inputOutput) override ;

    protected:
        virtual void validateParams() override ;
    }; // class ImageScaleProc


    /** Processor to select a window from an image.
     * The window can be partially overlapping with the image, in which
     * case the 'fill' parameter should be provided.
     * An special case of 'crop' mode allows to easily crop
     * a given amount of pixels from the sides of the image
     * (left, top, right, bottom).
     */
    class ImageWindowProc: public ImageProcessor
    {
        using ImageProcessor::ImageProcessor;

    public:
        enum OP {OP_CROP, OP_WINDOW};

        /** Scale input image and store the new one in output.
         * The output image will have the dimension defined by param "newdim"
         */
        virtual void process(const Image &input, Image &output) override ;

        /** Apply the scale and store the output in the same input image.
         */
        virtual void process(Image &inputOutput) override ;

    protected:
        virtual void validateParams() override ;
    }; // class ImageScaleProc

} // namespace emcore

#endif //EM_CORE_PROCESSOR_H
