//
// Created by josem on 1/7/17.
//

#include <iostream>
#include <sstream>
#include <cassert>

#include "em/base/type.h"
#include "em/base/array.h"
#include "em/image/image.h"


using namespace em;


// ===================== ImageImpl Implementation =======================

class ImageImpl
{
public:
    ArrayBase * arrayPtr = nullptr;
    Type * typePtr = nullptr;

};


// ===================== Image Implementation =======================


