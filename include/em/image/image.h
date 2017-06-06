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
    class Image
    {
    public:
        Image();
        virtual ~Image();

        // Dimensions
        void resize(const ArrayDim &adim, const Type *type);
        ArrayDim getDimensions() const;

        // Operators
        template <class T>
        Image& operator=(const T);
        // Return a raw pointer to data but doing some type checking
        template <class T>
        T * getDataPointer() const;

    private:
        // Pointer to implementation class, PIMPL idiom
        ImageImpl * implPtr;
    }; // class Image


}


#endif //EM_CORE_IMAGE_H
