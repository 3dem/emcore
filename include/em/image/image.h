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

        // This method should be called to register a ImageIO that is able
        // to read a new image format
        static bool registerIO(const ImageIO *reader);
        // Check if there is a registered reader for a given extension
        static bool hasIO(const std::string &extOrName);
        // Retrieve an existing reader for a given extension
        static ImageIO* getIO(const std::string &extOrName);

    private:
        // Pointer to implementation class, PIMPL idiom
        ImageImpl * implPtr;
    }; // class Image

    std::ostream& operator<< (std::ostream &ostream, const em::Image &t);


    /** @ingroup image
     * This class represent the location of one or several images in disk.
     * It contains a path to a physical file on disk, and a given index
     * or range. In EM, many images are usually grouped in a single file (stack).
     * So we need to store the path and the indexes of the image(s).
     */
    class ImageLocation
    {
    public:
        // TODO: maybe consider a pointer to string, so many ImageLocation objects
        // could share the same path string without extra memory
        std::string path;
        size_t start; ///< Starting index to read from file (first one is 1)
        size_t end; ///< Last index to read (0 means just the starting one, -1 means till the end)
    }; // class ImageLocation


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
        /** Return a name identifying this reader. */
        virtual std::string getName() const = 0;

        /** Return the extensions this reader is able to read. */
        virtual std::string getExtensions() const = 0;

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

        virtual void openFile(const std::string &path);
        virtual void read(const size_t index, Image &image) = 0;
        virtual void closeFile();

        virtual ~ImageIO();

    protected:
        /** Create an instance of ImageHandler (or subclass).
         * @return A pointer to the given instance of the handler.
         */
        virtual ImageHandler* createHandler();

        /** Reader the main header of an image file */
        virtual void readHeader() = 0;

        /** Clone this reader and obtain a new copy.
         * The caller to this functions should take care
         * of memory disposal. This function should only be accessible
         * Image class when retrieving a registered reader.
         *
         * @return
         */
        virtual ImageIO * create() const = 0;

        ImageHandler* handler = nullptr;

        friend class Image;
    }; // class ImageIO


    /** Helper class to store information about image file.
     * This class can only be used from ImageIO class.
     */
    class ImageHandler
    {
    protected:
        ImageHandler() {}; // avoid instances of this class

    public:
        // Store the name of the file that was read/written
        std::string path;
        // Keep a file handler to the image file
        FILE* file;
        // Store dimensions of the image file
        ArrayDim dim;

        friend class ImageIO;
    }; // class ImageHandler




} // em namespace


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_IMAGE_IO(ioClassName) \
    ioClassName reader_##ioClassName; \
    bool register_##ioClassName = Image::registerIO(&reader_##ioClassName)

#endif //EM_CORE_IMAGE_H
