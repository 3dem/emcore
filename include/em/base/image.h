//
// Created by josem on 1/7/17.
//

#ifndef EM_CORE_IMAGE_H
#define EM_CORE_IMAGE_H

#include <cstddef>
#include <cstdio>
#include <string>

#include "em/base/string.h"
#include "em/base/object.h"
#include "em/base/array.h"
#include "em/os/filesystem.h"


namespace em
{
    /** @ingroup image
     * This class represent the location of one or several images in disk.
     * It contains a path to a physical file on disk, and a given index.
     * In EM, many images are usually grouped in one single file (stack).
     * So we need to store the path and the index of the image.
     */
    class ImageLocation
    {
    public:
        /***
         *  Possible indexes contansts to be used in ImageLocation
         */
        enum Index
        {
            NONE = -1, ///< Index not set
            ALL = 0,   ///< All indexes available
            FIRST = 1  ///< The first index
        };

        // TODO: maybe consider a pointer to string, so many ImageLocation objects
        // could share the same path string without extra memory
        std::string path;
        size_t index; ///< Index to read from file (first one is 1, 0 means all images)

        ImageLocation() = default;
        ImageLocation(const std::string &path, size_t index=ALL);

        // Comparison operators
        bool operator==(const ImageLocation &other) const;
        bool operator!=(const ImageLocation &other) const;

        /** Simple string representation */
        std::string toString() const;
    }; // class ImageLocation

    std::ostream& operator<< (std::ostream &ostream, const ImageLocation &loc);

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
        // TODO: In C++ 11 the base constructor can be generated
        Image(const ArrayDim &adim, const Type & type);

        /** Copy constructor from another Array.
         * This Array will have the same dimensions, data type
         * and values of the other Array.
         * @param other Other Array to be copied
         */
        Image(const Image &other);
        virtual ~Image();

        Image& operator=(const Image &other);

        /** Return the header of a given image.
         *
         * @param index If 0, return the main header, if not, the specified one
         * @return ObjectDict reference of the required header
         */
        ObjectDict& getHeader(size_t index=0);

        // String representation
        virtual void toStream(std::ostream &ostream) const override;

        /** Read image data from a given location.
         * This function is a shortcut to easily read an image from a location
         * without using the ImageIO class.
         * The file will be open before data is read and closed after it.
         * If you want to read multiple images from the same file, it
         * would be better to first open the file explicitly using ImageIO,
         * read all the images and then close the file.
         * @param location Input image location (index range and path) to be read
         */
        void read(const ImageLocation &location);

        /** Write the image data into a file location.
         * This function is a shortcut to easily write an image without
         * using the ImageIO class.
         * @param location Input location where the image will be written.
         */
        void write(const ImageLocation &location) const;


    private:
        // Pointer to implementation class, PIMPL idiom
        class Impl;
        Impl * impl;
    }; // class Image

    std::ostream& operator<< (std::ostream &ostream, const em::Image &image);
    std::istream& operator>> (std::istream &istream, em::Image &image);

    /** @ingroup image
     * Class that will take care of read/write images from/to file.
     *
     * Internally, the ImageIO class holds a pointer to ImageIO::Impl class,
     * that contains the details about how to open files and read the images
     * data. This class contains some basic functionality that is shared
     * among some formats. The ImageIO::Impl class should be extended to provide
     * support for other formats.
     */
    class ImageIO
    {
    public:
        /** Implementation sub-class, it should be overriden to support other
         * formats */
        class Impl;

        /** Used when registering new Impl classes.
         * The ImplBuilder is a function that should return a pointer to
         * a newly created implementation.
         */
        using ImplBuilder = Impl* (*)();



        /**
         * Empty constructor for ImageIO.
         * In this case the newly created instance will have no format
         * implementation associated to read/write formats. Then, when the
         * open() method is called to open a file, the format implementation
         * will be inferred from the filename extension. Some functions will
         * raise an exception if called without having opened a file and,
         * therefore, without having an underlying format implementation.
         */
        ImageIO();

        /**
         * Constructor to build a new ImageIO instance given its name or
         * an extension related to the format implementation. The provided
         * input string should be the key associated to a know format
         * implementation. If not, an exception will be thrown. If the format
         * implementation is associated to the ImageIO instance, it will not
         * change when calling the open() method. This allow to read/write
         * images with unknown (or non-standard) file extensions.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered for it.
         */
        ImageIO(const std::string &extOrName);

        /**
         * Check if some ImageIO implementation is registered for a given name
         * or extension.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered.
         * @return Return True if there is any ImageIO registered.
         */
        static bool hasImpl(const std::string &extOrName);

        /**
         * Register a new ImageIO implementation.
         * This function should not be used unless you are developing an
         * implementation for a new ImageIO format.
         */
         static bool registerImpl(const StringVector &extOrNames,
                                  ImplBuilder builder);

        /** Return the dimensions of the file opened. */
        ArrayDim getDim() const;

        // TODO: DOCUMENT
        void open(const std::string &path,
                  const File::Mode mode=File::Mode::READ_ONLY);
        // TODO: DOCUMENT
        void close();

        /**
         * Create an empty file with a given dimensions and a given type.
         * This function should be used once and only when the file was opened
         * with TRUNCATE mode (also with READ_WRITE when the file does not exist).
         * @param adim The dimensions of the new file to be created.
         * @param type The data type of the elements that will be in the file.
         */
        void createFile(const ArrayDim &adim, const Type & type);

        /**
         * Expand the current file for adding more elements.
         * The file needs to be opened with READ_WRITE mode and it should
         * exists. The provided number of elements should be greater that
         * the current ndim of the file.
         * @param ndim The new number of desired elements in the file.
         */
        void expandFile(const size_t ndim);

        // TODO: DOCUMENT
        void read(size_t index, Image &image);

        // TODO: DOCUMENT
        void write(size_t index, const Image &image);

        /**
         * Print information about the image file.
         * @param verbosity 0 means silent, so nothing will be printed
         * if it is 1, only the basic information will be shown. If > 1,
         * some extra information will be provided.
         */
        void toStream(std::ostream &ostream, int verbosity=1) const;

        ~ImageIO();

        /**
         * Read from file and swap the data if needed
         * @param file File handler
         * @param data Pointer to data
         * @param count Number of data elements
         * @param typeSize Number of bytes for each element
         * @param swap Boolean to either swap or not the data array
         * @return
         */
        static size_t fread(FILE *file, void *data, size_t count,
                            size_t typeSize, bool swap = false);

        /**
         * Read from file to Array and swap the data if needed
         * @param file File handler
         * @param array object Array to copy data from file
         * @param swap Boolean to either swap or not the data array
         * @return
         */
        static size_t fread(FILE *file, Array &array, bool swap = false);


    private:
        // Pointer to implementation class, PIMPL idiom
        Impl* impl = nullptr;

    }; // class ImageIO

    std::ostream& operator<< (std::ostream &ostream, const em::ImageIO &t);

} // em namespace


#endif //EM_CORE_IMAGE_H
