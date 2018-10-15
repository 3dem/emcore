//
// Created by josem on 9/28/17.
//

#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

//FIXME Check in which OS glob works
#include <glob.h>

#include "em/os/filesystem.h"
#include "em/base/error.h"




using namespace em;


// ===================== File methods Implementation =======================

void File::resize(FILE *file, size_t size)
{
    int fno = fileno(file);
    ftruncate(fno, size);
} // function File::resizeFile

const char* File::modeToString(Mode mode)
{
    const char * openMode = "r";

    switch (mode)
    {
        case  File::Mode::READ_WRITE:
            openMode = "r+"; break;
        case  File::Mode::TRUNCATE:
            openMode = "w"; break;
    }
    return openMode;
} // function File::modeToString


// ===================== File methods Implementation =======================

bool Path::exists(const std::string &path)
{
    struct stat buffer;

    return (stat(path.c_str(), &buffer) == 0);
} // function Path::exists

size_t Path::getFileSize(const std::string &path)
{
    struct stat buffer;
    // Return 0 if the file does not exists
    return (stat(path.c_str(), &buffer) ? 0 : buffer.st_size);
} // function Path::getFileSize

int Path::remove(const std::string &path)
{
    if (!exists(path))
        return 0; // it is fine if the file does not exists

    return ::remove(path.c_str());
} // function Path::remove


char Path::separator = '/';

std::string Path::getDirName(const std::string &path)
{
    size_t pos = path.find_last_of(separator);
    return pos == std::string::npos ? "" : path.substr(0, pos);
} // function Path::getDirName

std::string Path::getFileName(const std::string &path)
{
    size_t pos = path.find_last_of(separator);
    return pos == std::string::npos ? path : path.substr(pos+1, path.size());
} // function Path::getFileName

std::string Path::getExtension(const std::string &path)
{
    std::string fn = getFileName(path);
    size_t pos = fn.find_last_of('.');
    return pos == std::string::npos ? "" : fn.substr(pos+1, path.size());
} // function Path::getExtension

std::string Path::removeExtension(const std::string &path)
{
    return path.substr(0, path.find_last_of('.'));
}

class Glob::Impl
{
public:
    glob_t results;

    ~Impl()
    {
        globfree(&results);
    }
};


Glob::Glob(const std::string &pattern)
{
    impl = new Impl();
    glob(pattern.c_str(), 0, 0, &impl->results);
}

Glob::~Glob()
{
    delete impl;
}

size_t Glob::getSize() const
{
    return impl->results.gl_pathc;
}

std::string Glob::getResult(size_t i) const
{
    if (i < 0 || i >= getSize())
        THROW_ERROR("Invalid index");

    return impl->results.gl_pathv[i];
}