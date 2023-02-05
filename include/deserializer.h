#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef _WIN32
#include <dirent.h>
#define MKDIR(path, mode) mkdir(path, mode)
#elif _WIN32
#include <windows.h>
#define MKDIR(path, mode) _mkdir(path)
#include "dirent_win.h"
#endif

bool mkdir_recursive(const char *path);

#endif // DESERIALIZER_H