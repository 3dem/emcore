//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//


#include "gtest/gtest.h"

#include "em/proc/processor.h"


using namespace em;


TEST(ImageOperator, Basic)
{
    ImageMathProc imgOp;
    ImageLocation loc;
    std::map<std::string, ArrayDim> fileDims;
    auto testDataPath = getenv("EM_TEST_DATA");

    if (testDataPath != nullptr)
    {
        try
        {
            std::string root(testDataPath);
            root += "emx/alignment/testAngles/";

            fileDims["sphere_128.vol"] = ArrayDim(128, 128, 128, 1);
            fileDims["proj.spi"] = ArrayDim(128, 128, 1, 1);

            Image img, img2, img3;

            for (auto &pair: fileDims)
            {
                imgOp[ImageMathProc::OPERATION] = Type::ADD;
                imgOp[ImageMathProc::OPERAND] = 100.f;
                loc.index = 1;
                std::cerr << ">>>>>>>>>> Reading " << pair.first << std::endl;
                loc.path = root + pair.first;
                img.read(loc);
                ArrayDim imgDim(pair.second);
                imgDim.n = 1;
                ASSERT_EQ(img.getDim(), imgDim);

                std::cerr << ">>>>>>>>>> Adding " << pair.first << std::endl;
                imgOp[ImageMathProc::OPERATION] = Type::ADD;
                imgOp.process(img, img2);
                ASSERT_EQ(img2.getDim(), imgDim);
                std::string outFn = pair.first + std::string("_add.mrc");
                std::cout << "     Writing ADD output to: " << outFn << std::endl;
                std::cout << "img2: " << img2 << std::endl;
                img2.write(ImageLocation(outFn, 1));

                std::cerr << ">>>>>>>>>> Multiplying " << pair.first << std::endl;
                imgOp[ImageMathProc::OPERATION] = Type::MUL;
                imgOp.process(img, img2);
                ASSERT_EQ(img2.getDim(), imgDim);
                outFn = pair.first + std::string("_mul.mrc");
                std::cout << "     Writing MUL output to: " << outFn << std::endl;
                std::cout << "img2: " << img2 << std::endl;
                img2.write(outFn);

                imgOp[ImageMathProc::OPERATION] = Type::ADD;
                imgOp[ImageMathProc::OPERAND] = img2;
                imgOp.process(img, img3);
                outFn = pair.first + std::string("_add-image.mrc");
                std::cout << "     Writing ADD-Img output to: " << outFn << std::endl;

                std::cout << "img3: " << img3 << std::endl;
                img3.write(outFn);
            }
        }
        catch (Error &err)
        {
            std::cout << err << std::endl;
        }
    }
    else
    {
        std::cout << "Skipping image processing tests, EM_TEST_DATA not "
                     "defined in environment. " << std::endl;
    }
} // TEST Column.Basic

