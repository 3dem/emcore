//
// Created by josem on 9/28/17.
//

#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

#include "em/os/filesystem.h"

using namespace em;


// ===================== File methods Implementation =======================

void File::resize(FILE *file, size_t size)
{
    int fno = fileno(file);
    ftruncate(fno, size);
} // function File::resizeFile


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