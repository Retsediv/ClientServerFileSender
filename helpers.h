#ifndef EZLOTESTTASK_HELPERS_H
#define EZLOTESTTASK_HELPERS_H

#include <fstream>
#include <sys/stat.h>

bool file_exists(const char *filename);

bool is_regular_file(const char *path);

bool is_directory(const char *path);

#endif
