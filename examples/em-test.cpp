// A simple program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "emc/base/error.h"
#include "emc/base/image.h"
#include "emc/base/type.h"
#include "emc/proc/processor.h"
#include  <map>

using namespace emcore;


int main(int argc, char *argv[])
{
    std::string inputFn = "006.em";
    std::string outputFn = "006p.em";

    Image img, imgOut;
    img.read(inputFn);
    ImageMathProc proc;
    proc[ImageProcessor::OPERATION] = Type::ADD;
    proc[ImageMathProc::OPERAND] = 1000;
    ImagePipeProc pipeProc;
    pipeProc.addProcessor(&proc);
    pipeProc.process(img, imgOut);
    imgOut.write(outputFn);


    return 0;
}
