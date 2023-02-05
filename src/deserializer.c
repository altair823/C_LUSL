#include "deserializer.h"

bool mkdir_recursive(const char *path) {
    if (path == NULL) {
        return false;
    }
    char *path_copy = (char *) malloc(sizeof(char *) * (strlen(path) + 1));
    memcpy(path_copy, path, strlen(path) + 1);
    char *path_ptr = path_copy;
    while (*path_ptr != '\0') {
        if (*path_ptr == '/') {
            *path_ptr = '\0';
            if (MKDIR(path_copy, 0777) == -1 && errno != EEXIST) {
                free(path_copy);
                return false;
            }
            *path_ptr = '/';
        }
        path_ptr++;
    }
    if (MKDIR(path_copy, 0777) == -1 && errno != EEXIST) {
        free(path_copy);
        return false;
    }
    free(path_copy);
    return true;
}