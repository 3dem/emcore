//
// Created by josem on 11/7/17.
//

#ifndef EM_CORE_PROCESSOR_H
#define EM_CORE_PROCESSOR_H

#include "em/image/image.h"


namespace em
{


    /**
     * Base class for all operations that receive an Image as input and produce
     * an Image as output.
     *
     */
    class ImageProcessor
    {
    public:
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

    }; // class ImageProcessor

} // namespace em

#endif //EM_CORE_PROCESSOR_H
