//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_H
#define EM_CORE_IMAGE_H

#include <cstddef>
#include <cstdio>
#include <string>

#include "em/base/object.h"
#include "em/base/array.h"


class ImageImpl;


namespace em
{

    class ImageIO;
    class ImageHandler;

    /** @ingroup image
     * Image class
     */
    class Image: public Array
    {
    public:
        /** Default constructor.
         * The Array will not have any data associated and
         * the type will be nullptr.
         */
        Image();

        // Constructor from dimensions and type
        Image(const ArrayDim &adim, ConstTypePtr type);

        /** Copy constructor from another Array.
         * This Array will have the same dimensions, data type
         * and values of the other Array.
         * @param other Other Array to be copied
         */
        Image(const Image &other);
        virtual ~Image();

        /** Return the header of a given image.
         *
         * @param index If 0, return the main header, if not, the specified one
         * @return ObjectDict reference of the required header
         */
        ObjectDict& getHeader(size_t index=0);

        // String representation
        virtual void toStream(std::ostream &ostream) const override;


    private:
        // Pointer to implementation class, PIMPL idiom
        ImageImpl * implPtr;
    }; // class Image

    std::ostream& operator<< (std::ostream &ostream, const em::Image &t);


    /** @ingroup image
     * This class represent the location of one or several images in disk.
     * It contains a path to a physical file on disk, and a given index.
     * In EM, many images are usually grouped in a single file (stack).
     * So we need to store the path and the index of the image.
     */
    class ImageLocation
    {
    public:
        // TODO: maybe consider a pointer to string, so many ImageLocation objects
        // could share the same path string without extra memory
        std::string path;
        size_t index; ///< Index to read from file (first one is 1, 0 means all images)
    }; // class ImageLocation


    using FileMode = const uint8_t;

    /** @ingroup image
     * Base class to read Image from disk.
     *
     * Sub-classes of ImageReader should be implemented for reading
     * EM formats such as: spider, mrc, img, etc.
     * Other standard formats should also be supported,
     * including: tiff, png, jpeg, hdf5
     */
    class ImageIO
    {
    public:
        /** Constants for open files. */
        static const FileMode READ_ONLY = 0;
        static const FileMode READ_WRITE = 1;
        static const FileMode TRUNCATE = 2;

        // ---- Static methods related to ImageIO instances --------
        /**
         * Register a ImageIO class to be available for reading/writing images.
         * The class will be accessible via the ImageIO name and the extensions
         * defined by the class.
         * @param imgio Input pointer to the ImageIO subclass that will be
         * registred.
         * @return Return True if the new ImageIO was sucessfully registered.
         */
        static bool set(const ImageIO *imgio);

        /**
         * Check if some ImageIO is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered.
         * @return Return True if there is any ImageIO registered.
         */
        static bool has(const std::string &extOrName);

        /**
         * Retrieve an ImageIO instance for a given name or extension.
         * @param extOrName Input string representing either the ImageIO name
         * @return Return a pointer to a new ImageIO instance.
         */
        static ImageIO* get(const std::string &extOrName);

        /** Return a name identifying this reader. */
        virtual std::string getName() const = 0;

        /** Return the extensions this reader is able to read. */
        virtual StringVector getExtensions() const = 0;

        /** Return the dimensions of the file opened. */
        ArrayDim getDimensions() const;

        // TODO: DOCUMENT
        virtual void open(const std::string &path, const FileMode mode=READ_ONLY);
        // TODO: DOCUMENT
        virtual void close();

        /**
         * Create an empty file with a given dimensions and a given type.
         * This function should be used once and only when the file was opened
         * with TRUNCATE mode (also with READ_WRITE when the file does not exist).
         * @param adim The dimensions of the new file to be created.
         * @param type The data type of the elements that will be in the file.
         */
        virtual void createFile(const ArrayDim &adim, ConstTypePtr type);

        /**
         * Expand the current file for adding more elements.
         * The file needs to be opened with READ_WRITE mode and it should
         * exists. The provided number of elements should be greater that
         * the current ndim of the file.
         * @param ndim The new number of desired elements in the file.
         */
        virtual void expandFile(const size_t ndim);

        // TODO: Move this basic function to the Image class as a shortcut
        /** Read a given image from file.
         * This function is the most basic way to read an image from disk.
         * The file will be open before data is read and close after it.
         * If you want to read multiple images from the same file, it
         * would be better to first open the file explicitly,
         * read the images and then close the file.
         * @param location Input image location (index range and path) to be read
         * @param image Image where data will be read
         */
        virtual void read(const ImageLocation &location, Image &image);

        // TODO: Move this basic function to the Image class as a shortcut
        // virtual void write(const ImageLocation &location, const Image &image);

        virtual void read(const size_t index, Image &image);
        virtual void write(const size_t index, const Image &image);

        virtual void readImageHeader(const size_t index, Image &image) = 0;
        virtual void writeImageHeader(const size_t index, Image &image) = 0;

        virtual ~ImageIO();

    protected:
        /** Create an instance of ImageHandler (or subclass).
         * @return A pointer to the given instance of the handler.
         */
        virtual ImageHandler* createHandler();

        /** Read the main header of an image file */
        virtual void readHeader() = 0;

        /** Write the main header of an image file */
        virtual void writeHeader() = 0;

        /** Return the size of the header for this format */
        virtual size_t getHeaderSize() const = 0;

        /** Return the size of padding between images/volumes in a stack */
        virtual size_t getPadSize() const;

        /** Clone this reader and obtain a new copy.
         * The caller to this functions should take care
         * of memory disposal. This function should only be accessible
         * Image class when retrieving a registered reader.
         *
         * @return
         */
        virtual ImageIO * create() const = 0;

        ImageHandler* handler = nullptr;

    private:
        static std::map<std::string, const ImageIO*> iomap;
    }; // class ImageIO


} // em namespace


#endif //EM_CORE_IMAGE_H
