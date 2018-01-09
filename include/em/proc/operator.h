//
// Created by josem on 11/7/17.
//

#ifndef EM_CORE_OPERATOR_H
#define EM_CORE_OPERATOR_H

#include <map>
#include "em/proc/processor.h"


namespace em
{

    /**
     * Processor to perform some basic image operations such as:
     * addition, subtraction, multiplication and division
     */
    class ImageMathProc: public ImageProcessor
    {
    public:
        static const std::string OPERATION;
        static const std::string OPERAND;

        // FIXME: Using ENUM provoke a compilation error due to instantiation
        // FIXME: of Type::get<Operation> and it does not have operator>> used in Type
        // enum Operation {ADD, SUB, MUL, DIV};
        using Operation = int8_t ;
        static const Operation ADD;
        static const Operation SUB;
        static const Operation MUL;
        static const Operation DIV;

        ImageMathProc();

        /** Apply the operation defined by this Processor to the input
         * image and store the result in the output image.
         */
        virtual void process(const Image &input, Image &output) override ;

        /** Apply the operation defined by this Processor to input image
         * and modify it to store the result.
         * @param inputOutput
         */
        virtual void process(Image &inputOutput) override ;

    private:
        /** Define a function pointer type to process images */
        using ProcessFunc = void (*)(Image &, const Object&, Operation);

        std::map<ConstTypePtr, ProcessFunc> map;
    }; // class ImageMathProc

} // namespace em

#endif //EM_CORE_OPERATOR_H
