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
    class ImageIO::Impl
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
        Type type;

        // Swap bytes in case file Endian differs from local Endian
        bool swap = false;

        // Return the size of padding between images/volumes in a stack
        size_t pad = 0;

        Image image; ///< Temporary image used as buffer to read from disk

        // ---- Static methods related to ImageIO instances --------
        /**
         * Register a ImageIO class to be available for reading/writing images.
         * The class will be accessible via the ImageIO name and the extensions
         * defined by the class.
         * @param imgio Input pointer to the ImageIO subclass that will be
         * registred.
         * @return Return True if the new ImageIO was successfully registered.
         */
        static bool registerImpl(const ImageIO *imgio);

        /**
         * Check if some ImageIO is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered.
         * @return Return True if there is any ImageIO registered.
         */
        static bool hasImpl(const std::string &extOrName);

        friend class ImageIO;

        virtual ~Impl();

    protected:
        /** Read the main header of an image file */
        virtual void readHeader() = 0;

        /** Write the main header of an image file */
        virtual void writeHeader() = 0;

        /** Return the size of the header for this format */
        virtual size_t getHeaderSize() const;

        /** Return the size of padding between images/volumes in a stack */
        virtual size_t getPadSize() const;

        /** Return the size of the image.
         * Usually this will be the product: x * y * z * typeSize + padSize
         */
        virtual size_t getImageSize() const;

        /**
         * Return the string file open mode from our numerical
         * open mode value. This could be format-specific.
         */
        const char * getOpenMode(FileMode mode) const;

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
        virtual const TypeMap& getTypeMap() const = 0;

        const Type& getTypeFromMode(int mode) const;
        int getModeFromType(const Type &type) const;


        /**
         * Print information about the image file.
         * @param verbosity 0 means silent, so nothing will be printed
         * if it is 1, only the basic information will be shown. If > 1,
         * some extra information will be provided.
         */
        virtual void toStream(std::ostream &ostream, int verbosity=1) const;

    private:
        // Store a mapping between format names (mrc, spider, etc)
        // and a function that helps to build a new ImageIO::Impl
        static std::map<std::string, const ImageIO*> * implMap;
    }; // class ImageIO::Impl

    using ImageIOImplBuilder = ImageIO::Impl* (*)();
    bool registerImageIOImpl(const StringVector &sv, ImageIOImplBuilder builder);


} // em namespace


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_IMAGE_IO(extensions, ioClassName) \
    ImageIO::Impl * new___##ioClassName(){ return new ioClassName(); } \
    bool reg___##ioClassName = em::registerImageIOImpl(extensions, new___##ioClassName)

#endif //EM_CORE_IMAGE_PRIV_H
