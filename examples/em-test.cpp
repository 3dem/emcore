// A simple program

#include "emc/proc/processor.h"

using namespace emcore;


int main(int argc, char *argv[])
{
    std::string inputFn = "006.mrc";
    std::string outputFn = "006_inverted_scaled.mrc";

    Image img, imgOut;
    img.read(inputFn);

    ImageMathProc invProc;

    ImagePipeProc pipeProc;
    pipeProc.addProcessor(new ImageMathProc({{"operation", Type::MUL},
                                             {"operand", -1}}));
    pipeProc.addProcessor(new ImageScaleProc({{"factor", 0.5}}));

    pipeProc.process(img, imgOut);
    imgOut.write(outputFn);

    return 0;
}
