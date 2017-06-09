//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_H
#define EM_CORE_IMAGE_H

#include <cstddef>
#include <string>

#include "em/base/object.h"
#include "em/base/array.h"


class ImageImpl;


namespace em
{

    class ImageReader;
    class ImageWriter;

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

        // This method should be called to register a ImageReader that is able
        // to read a new image format
        static bool registerReader(const ImageReader * reader);
        // Check if there is a registered reader for a given extension
        static bool hasReader(const std::string &extension);
        // Retrieve an existing reader for a given extension
        static ImageReader* getReader(const std::string &extension);

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
     * form the EM formats such as: spider, mrc, img, etc.
     * Other more standard formats should also be supported,
     * including: tiff, png, jpeg, hdf5
     */
    class ImageReader
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
        virtual void read(const ImageLocation &location, Image &image) = 0;

        virtual void openFile(const std::string &path) = 0;
        virtual void read(const size_t index, Image &image) = 0;
        virtual void closeFile() = 0;

    private:
        /** Clone this reader and obtain a new copy.
         * The caller to this functions should take care
         * of memory disposal. This function should only be accessible
         * Image class when retrieving a registered reader.
         *
         * @return
         */
        virtual ImageReader * create() const = 0;

        friend class Image;
    }; // class ImageReader

//extern bool reg;
} // em namespace


// The following macro can be used as a shortcut to register new ImageReader subclasses
#define REGISTER_IMAGE_READER(readerClassName) \
    readerClassName reader_##readerClassName; \
    bool register_##readerClassName = Image::registerReader(&reader_##readerClassName)

#endif //EM_CORE_IMAGE_H
