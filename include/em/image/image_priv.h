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
    class ImageIOImpl
    {
    public:
        // Store the name of the file that was read/written
        std::string path;

        // Mode in which the file was open
        FileMode fileMode = ImageIO::READ_ONLY;

        // Keep a file handler to the image file
        FILE* file = nullptr;

        // Store dimensions of the image file
        ArrayDim dim;

        // Detected datatype in the file
        ConstTypePtr type = nullptr;

        // Swap bytes in case file Endian differs from local Endian
        bool swap = false;

        // Return the size of padding between images/volumes in a stack
        size_t pad = 0;

        Image image; ///< Temporary image used as buffer to read from disk

        friend class ImageIO;

        virtual ~ImageIOImpl();

    protected:
        /**
         * Return the string file open mode from our numerical
         * open mode value. This could be format-specific.
         */
        const char * getOpenMode(FileMode mode) const;

        /** Open the file for this format. The path and mode
         * should be set before calling this function.
         */
        virtual void openFile();
    }; // class ImageIOImpl


} // em namespace


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_IMAGE_IO(ioClassName) \
    ioClassName reader_##ioClassName; \
    bool register_##ioClassName = ImageIO::set(&reader_##ioClassName)

#endif //EM_CORE_IMAGE_PRIV_H
