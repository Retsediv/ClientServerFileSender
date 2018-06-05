#ifndef EZLOTESTTASK_HEPERS_H
#define EZLOTESTTASK_HEPERS_H

#include <fstream>
#include <sys/stat.h>

bool file_exists(const char *filename) {
    std::ifstream ifile(filename);
    return (bool) ifile;
}

bool is_regular_file(const char *path) {
    struct stat s;
    return (stat(path, &s) == 0) && (s.st_mode & S_IFREG);
}

#endif
