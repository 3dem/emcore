//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_PRIV_H
#define EM_CORE_IMAGE_PRIV_H

#include "image.h"


namespace em
{
    /** Helper class to store information about image file.
     * This class can only be used from ImageIO class.
     */
    class ImageIO::Impl
    {
    public:
        // Store the name of the file that was read/written
        std::string path;

        // Mode in which the file was open
        File::Mode fileMode = File::Mode::READ_ONLY;

        // Keep a file handler to the image file
        FILE* file = nullptr;

        // Store dimensions of the image file
        ArrayDim dim;

        // Detected datatype in the file
        Type type;

        // Swap bytes in case file Endian differs from local Endian
        bool swap = false;

        // Return the size of padding between images/volumes in a stack
        size_t pad = 0;

        Image image; ///< Temporary image used as buffer to read from disk

        friend class ImageIO;

        virtual ~Impl();

        /** Return a list of the supported types of this implementation. */
        TypeVector getTypes() const;

    protected:
        /** Read the main header of an image file */
        virtual void readHeader() = 0;

        /** Write the main header of an image file */
        virtual void writeHeader() = 0;

        /** Return the size of the header for this format */
        virtual size_t getHeaderSize() const;

        /** Return the size of padding between images/volumes in a stack */
        virtual size_t getPadSize() const;

        /** Return the size of the image in bytes (including pad size).
         * Usually this will be the product: x * y * z * typeSize + padSize
         */
        virtual size_t getImageSize() const;

        /**
         * Return the string file open mode from our numerical
         * open mode value. This could be format-specific.
         */
        const char * getModeString() const;

        /** Open the file for this format. The path and mode
         * should be set before calling this function.
         */
        virtual void openFile();
        virtual void closeFile();

        /**
         * Expand the existing file with unset values.
         * This function should be called after setting dim and type.
         */
        virtual void expandFile();

        virtual void readImageHeader(const size_t index, Image &image);
        virtual void writeImageHeader(const size_t index, const Image &image);

        virtual void readImageData(const size_t index, Image &image);
        virtual void writeImageData(const size_t index, const Image &image);

        /**
         * Return a map between the format integer modes and the supported
         * Types for storing images.
         */
        virtual const IntTypeMap& getTypeMap() const = 0;

        Type getTypeFromMode(int mode) const;
        int getModeFromType(const Type &type) const;

        /**
         * Print information about the image file.
         * @param verbosity 0 means silent, so nothing will be printed
         * if it is 1, only the basic information will be shown. If > 1,
         * some extra information will be provided.
         */
        virtual void toStream(std::ostream &ostream, int verbosity=1) const;

    }; // class ImageIO::Impl

} // em namespace


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_IMAGE_IO(extensions, ioClassName) \
    ImageIO::Impl * new___##ioClassName(){ return new ioClassName(); } \
    bool reg___##ioClassName = em::ImageIO::registerImpl(extensions, new___##ioClassName)

#endif //EM_CORE_IMAGE_PRIV_H
