//
// Created by Jose Miguel de la Rosa Trevin on 2017-10-15.
//

#include <random>
#include "gtest/gtest.h"

#include "em/proc/processor.h"
#include "em/proc/stats.h"
#include "em/math/functions.h"
#include "em/base/timer.h"




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
} // TEST ImageOperator.Basic

TEST(Stats, Basic)
{
    size_t xdim = 1000;
     Array array(ArrayDim(xdim, xdim), typeFloat);
    int c = 1;
    array.set(c);
    auto s1 = Stats::compute(array);
ASSERT_FLOAT_EQ(s1.min, c);
ASSERT_FLOAT_EQ(s1.max, c);
ASSERT_FLOAT_EQ(s1.mean, c);
ASSERT_FLOAT_EQ(s1.std, 0);

    std::default_random_engine gen;
    std::normal_distribution<float> dist(5.0,2.0);

    auto data = array.getView<float>().getData();
    for (size_t i = 0; i < array.getDim().getSize(); ++i)
        data[i] = dist(gen);

    Timer t;
    t.tic();
    s1 = Stats::compute(array);
//ASSERT_EQ(s1.min, c);
//ASSERT_EQ(s1.max, c);
float error = 0.01;
ASSERT_NEAR(s1.mean, 5.0, error);
ASSERT_NEAR(s1.std, 2.0, error);

    t.toc("computed stats (ALL)");

    t.tic();
    s1 = Stats::compute(array, Stats::MIN_MAX);
    t.toc("computed stats (MIN_MAX)");

    // Assing 1, 2, 3, 4, 5 to the first values
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    data[3] = 4;
    data[4] = 5;
    s1 = Stats::compute(typeFloat, data, 5);
ASSERT_FLOAT_EQ(s1.min, 1);
ASSERT_FLOAT_EQ(s1.max, 5);
ASSERT_FLOAT_EQ(s1.mean, 3);
ASSERT_NEAR(s1.std, 1.4142, error);
}