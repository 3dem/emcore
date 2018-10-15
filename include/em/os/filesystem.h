//
// Created by josem on 9/28/17.
//

#ifndef EM_CORE_FILE_H
#define EM_CORE_FILE_H

#include <string>


namespace em
{
    /**
     * Class to contain some static functions for File utility.
     */
    class File
    {
    public:
        static void resize(FILE *file, size_t size);

        /** Enum to simplify the options to open files */
        enum Mode { READ_ONLY = 0, READ_WRITE = 1, TRUNCATE = 2 };

        /** Return the C equivalent from our file open modes.
         * READ_ONLY = 'r'
         * READ_WRITE = 'r+'
         * TRUNCATE = 'w'
         */
        static const char * modeToString(Mode mode);

    }; // class File

    class Path
    {
    public:
        static char separator;
        static std::string getDirName(const std::string &path);
        static std::string getFileName(const std::string &path);
        static std::string getExtension(const std::string &path);
        static std::string removeExtension(const std::string &path);

        static bool exists(const std::string &path);
        static size_t getFileSize(const std::string &path);

        static int remove(const std::string &path);
    }; // class Path

    /**
     * Class to wrap the glob function and present a nicer API.
     */
     class Glob
     {
     public:
         /** Object constructor based on a pattern. */
         Glob(const std::string& pattern);

         /** Destructor to clean internal data */
         ~Glob();

         /** Return the number of matched files.
          * It will return 0 if not file was matched.
          */
         size_t getSize() const;

         /** Return the a given result.
          * Valid indexes will between 0 and getSize()-1
          */
          std::string getResult(size_t i) const;

     private:
         class Impl;
         Impl* impl; // pointer to implementation
     };


} // namespace em

#endif //EM_CORE_FILE_H
