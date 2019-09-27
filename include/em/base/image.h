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


namespace emcore
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
         * without using the ImageFile class.
         * The file will be open before data is read and closed after it.
         * If you want to read multiple images from the same file, it
         * would be better to first open the file explicitly using ImageFile,
         * read all the images and then close the file.
         * @param location Input image location (index range and path) to be read
         */
        void read(const ImageLocation &location);

        /** Write the image data into a file location.
         * This function is a shortcut to easily write an image without
         * using the ImageFile class.
         * @param location Input location where the image will be written.
         */
        void write(const ImageLocation &location) const;


    private:
        // Pointer to implementation class, PIMPL idiom
        class Impl;
        Impl * impl;
    }; // class Image

    std::ostream& operator<< (std::ostream &ostream, const Image &image);
    std::istream& operator>> (std::istream &istream, Image &image);

    /** @ingroup image
     * Centralizes functionality to read/write image files from several EM formats.
     *
     * Internally, the ImageFile class holds a pointer to ImageFile::Impl class,
     * that contains the details about how to open files and read the images
     * data. The ImageFile class implements basic functionality that is common
     * to some formats. The ImageFile::Impl class is extended to provide support
     * for each EM format.
     */
    class ImageFile
    {
    public:
        /** Implementation sub-class, it should be overwritten to support
         * other formats */
        class Impl;

        /** Alias for a Type vector */
        using FormatTypes = std::map<std::string, TypeVector>;

        /** Used when registering new Impl classes.
         * The ImplBuilder is a function that should return a pointer to
         * a newly created implementation.
         */
        using ImplBuilder = Impl* (*)();

        /** Register a new ImageFile implementation.
         *
         * This function should not be used unless you are developing an
         * implementation for a new ImageFile format.
         */
        static bool registerImpl(const StringVector &extOrNames,
                                 ImplBuilder builder);

        /** Check if some implementation is registered for a given name
         * or extension.
         *
         * @param extOrName Input string representing either the ImageFile
         *      name or one of the extensions registered.
         * @return Return True if there is any implementation registered.
         */
        static bool hasImpl(const std::string &extOrName);

        /** Return data types supported by a given format implementation.
         *
         * An exception will be raised if the implementation can not be found,
         * so user needs to check that hasImpl returns true for this input.
         */
        static TypeVector getImplTypes(const std::string &extOrName);

        /** Return a map with all formats as key and their supported data types.
         */
        static FormatTypes getFormatTypes();

        /** Read from file and swap the data if needed.
         *
         * @param file File handler
         * @param data Pointer to data
         * @param count Number of data elements
         * @param typeSize Number of bytes for each element
         * @param swap Boolean to either swap or not the data array
         * @return
         */
        static size_t fread(FILE *file, void *data, size_t count,
                            size_t typeSize, bool swap=false);

        /** Read from file to Array and swap the data if needed.
         *
         * @param file File handler
         * @param array object Array to copy data from file
         * @param swap Boolean to either swap or not the data array
         * @return
         */
        static size_t fread(FILE *file, Array &array, bool swap=false);

        /** Empty constructor, no implementation set or file opened yet.
         *
         * In this case the newly created instance will have no format
         * implementation associated to read/write formats. Then, when the
         * open() method is called to open a file, the format implementation
         * will be inferred from the filename extension. Some functions will
         * raise an exception if called without having opened a file and,
         * therefore, without having an underlying format implementation.
         */
        ImageFile();

        /** Create a new ImageFile instance from a given path.
         *
         *  Equivalent to:
         *  auto imgFile = ImageFile();
         *  imgFile.open(path);
         *
         *  See ImageFile.open.
         */
        ImageFile(const std::string &path,
                  const File::Mode mode=File::Mode::READ_ONLY,
                  const std::string &formatName="");

        /** Open the image file from a given path.
         *
         * Raise an exception if the ImageFile is already opened.
         *
         * @param path The input path. If formatName is empty, the format
         *      will be inferred from the extension of the file path. If there
         *      is no implemented registered for that extension, an exception
         *      will be raised.
         * @param mode File mode to open the file. By default is READ_ONLY and
         *      the input path should exists. TRUNCATE can also be used to create
         *      a new file or overwrite an existing one.
         * @param formatName This parameter can be used to explicitly provide
         *      the name of the format to retrieve the registered implementation.
         *      If empty, the format will be inferred from the input path. This
         *      option can be useful for operation with filename without common
         *      format extensions.
         */
        void open(const std::string &path,
                  const File::Mode mode=File::Mode::READ_ONLY,
                  const std::string &formatName="");

        /** Return the dimensions of the opened file. */
        ArrayDim getDim() const;

        /** Return the type of the opened file */
        Type getType() const;

        /** Read an image from an already opened ImageFile.
         *
         * @param index Should be greater that 1 and less or equal to the
         *      number of images stored in the file.
         * @param image Output image object that will be used read the
         *      data from file. The type and dimensions of this output
         *      Image can be modified if needed to fit the data read.
         */
        void read(size_t index, Image &image);

        // TODO: DOCUMENT
        void write(size_t index, const Image &image);

        /** Create an empty file with a given dimensions and type.
         *
         * This function should be used once and only when the file was opened
         * with TRUNCATE mode (also with READ_WRITE when the file does not exist).
         * @param adim The dimensions of the new file to be created.
         * @param type The data type of the elements that will be in the file.
         */
        void createEmpty(const ArrayDim &adim, const Type & type);

        /** Expand the current file to add more elements.
         *
         * The file needs to be opened with READ_WRITE mode and it should
         * exists. The provided number of elements should be greater that
         * the current ndim of the file.
         * @param ndim The new number of desired elements in the file.
         */
        void expand(const size_t ndim);

        /** Print information about the image file.
         *
         * @param verbosity 0 means silent, so nothing will be printed
         * if it is 1, only the basic information will be shown. If > 1,
         * some extra information will be provided.
         */
        void toStream(std::ostream &ostream, int verbosity=1) const;

        /** Closed opened file and release internal data. */
        void close();

        /** Destructor */
        ~ImageFile();

    private:
        // Pointer to implementation class, PIMPL idiom
        Impl* impl = nullptr;

    }; // class ImageFile

    std::ostream& operator<< (std::ostream &ostream, const ImageFile &t);

} // em namespace


#endif //EM_CORE_IMAGE_H
