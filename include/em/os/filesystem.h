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


} // namespace em

#endif //EM_CORE_FILE_H
