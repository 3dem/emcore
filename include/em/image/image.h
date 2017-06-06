//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_H
#define EM_CORE_IMAGE_H

#include <cstddef>
#include <string>

#include "em/base/array.h"

class ImageImpl;


namespace em
{


    /** @ingroup image
     * Image class
     */
    class Image: public Array
    {
    public:
        Image();
        virtual ~Image();

    private:
        // Pointer to implementation class, PIMPL idiom
        ImageImpl * implPtr;
    }; // class Image


}


#endif //EM_CORE_IMAGE_H
