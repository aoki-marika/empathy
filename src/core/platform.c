#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>

// MARK: - Functions

char *platform_get_path()
{
    // read the executable path
    char buffer[PATH_MAX];
    readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    // copy the path into an allocated buffer and return it
    size_t path_size = strlen(buffer) + 1;
    char *path = (char *)malloc(path_size * sizeof(char));
    path[0] = '\0';
    strcat(path, buffer);
    return path;
}

char *platform_get_relative_path(const char *relative_path)
{
    // get the directory containing the executable
    char *executable_path = platform_get_path();
    char *directory = dirname(executable_path);

    // create the full path and return it
    // directory + slash + relative path + null terminator
    size_t path_size = strlen(directory) + 1 + strlen(relative_path) + 1;
    char *path = (char *)malloc(path_size * sizeof(char));
    sprintf(path, "%s/%s", directory, relative_path);
    return path;
}
