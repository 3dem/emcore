//
// Created by josem on 9/28/17.
//

#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>

#include "em/os/file.h"


void em::File::expand(FILE *file, size_t size)
{
    int fno = fileno(file);
    ftruncate(fno, size);
} // function File::expand

bool em::File::exists(const char *path)
{
    struct stat buffer;

    return (stat(path, &buffer) == 0);
} // function File::exists

size_t em::File::getSize(const char *path)
{
    struct stat buffer;
    // Return 0 if the file does not exists
    return (stat(path, &buffer) ? 0 : buffer.st_size);
} // function File::getSize

int em::File::remove(const char *path)
{
    // FIXME: Causing a Segmentation fault
    return remove(path);
} // function File::remove