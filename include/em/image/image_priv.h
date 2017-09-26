//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_PRIV_H
#define EM_CORE_IMAGE_PRIV_H

#include "em/image/image.h"


namespace em
{
    /** Helper class to store information about image file.
     * This class can only be used from ImageIO class.
     */
    class ImageHandler
    {
    protected:
        ImageHandler() = default; // avoid instances of this class

    public:
        // Store the name of the file that was read/written
        std::string path;
        // Keep a file handler to the image file
        FILE* file;
        // Store dimensions of the image file
        ArrayDim dim;
        // Detected datatype in the file
        ConstTypePtr type = nullptr;
        // Mode in which the file was open
        FileMode fileMode = ImageIO::READ_ONLY;

        Image image; ///< Temporary image used as buffer to read from disk

        friend class ImageIO;
    }; // class ImageHandler


} // em namespace


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_IMAGE_IO(ioClassName) \
    ioClassName reader_##ioClassName; \
    bool register_##ioClassName = ImageIO::set(&reader_##ioClassName)

#endif //EM_CORE_IMAGE_PRIV_H
