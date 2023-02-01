#include "serializer.h"

bool get_file_list(char *root_dir, stack_t *file_list) {
    if (root_dir == NULL || file_list == NULL) {
        return false;
    }
    INIT_STACK(dir_stack);
    PUSH_STACK(dir_stack, root_dir);
    while (dir_stack.top > 0) {
        char *subroot_dir = NULL;
        POP_STACK(dir_stack, subroot_dir);
        if (subroot_dir) {
            DIR *dir = opendir(subroot_dir);
            if (dir) {
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_DIR) {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                            char *path;
                            path = (char *) malloc(sizeof(char *) * (strlen(subroot_dir) + strlen(entry->d_name) + 2));
                            memcpy(path, subroot_dir, strlen(subroot_dir));
                            path[strlen(subroot_dir)] = '/';
                            memcpy(path + strlen(subroot_dir) + 1, entry->d_name, strlen(entry->d_name) + 1);
                            PUSH_STACK(dir_stack, path);
                        }
                    } else {
                        char *filename = (char *) malloc(sizeof(char *) * (strlen(subroot_dir) + strlen(entry->d_name) + 2));
                        memcpy(filename, subroot_dir, strlen(subroot_dir));
                        filename[strlen(subroot_dir)] = '/';
                        memcpy(filename + strlen(subroot_dir) + 1, entry->d_name, strlen(entry->d_name) + 1);
                        PUSH_STACK((*file_list), filename);
                    }
                }
                closedir(dir);
            }
        }
        if (subroot_dir && subroot_dir != root_dir) {
            free(subroot_dir);
        }
    }
    FREE_STACK(dir_stack);
    return true;
}