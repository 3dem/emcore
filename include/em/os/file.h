//
// Created by josem on 9/28/17.
//

#ifndef EM_CORE_FILE_H
#define EM_CORE_FILE_H

namespace em
{

    /**
     * Class to contain some static functions for File utility.
     */
    class File
    {
    public:
        static void expand(FILE * file, size_t size);
        static bool exists(const char * path);
        static size_t getSize(const char * path);
        static int remove(const char * path);

    }; // class File


} // namespace em

#endif //EM_CORE_FILE_H
