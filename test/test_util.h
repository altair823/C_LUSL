#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define INIT_TEST_SUITE printf("%s...", __func__)
#define END_TEST_SUITE printf("OK\n")

void make_directory(const char* name) {
    DIR *dir = opendir(name);
    if (dir) {
        closedir(dir);
        return;
    } else {
    #ifdef __linux__
        if (mkdir(name, 0777) == -1)
    #else
        if (_mkdir(name) == -1) 
    #endif
        {
            printf("Cannot create dir %s\n", name);\
            return;
        }
        else {
            return;
        }
    }
}

int _remove_directory(const char* name) {
    int success = -1;
    DIR *dir = opendir(name);
    if (dir) {
        struct dirent *entry = readdir(dir);
        while (!success && entry) {
            if (entry->d_type == DT_DIR) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    char *path;
                    path = (char *) malloc(sizeof(char *) * (strlen(name) + strlen(entry->d_name) + 2));
                    memcpy(path, name, strlen(name));
                    path[strlen(name)] = '/';
                    memcpy(path + strlen(name) + 1, entry->d_name, strlen(entry->d_name) + 1);
                    success = _remove_directory(path);
                    free(path);
                }
            } else {
                char *filename = (char *) malloc(sizeof(char *) * (strlen(name) + strlen(entry->d_name) + 2));
                memcpy(filename, name, strlen(name));
                filename[strlen(name)] = '/';
                memcpy(filename + strlen(name) + 1, entry->d_name, strlen(entry->d_name) + 1);
                success = remove(filename);
                free(filename);
            }
            entry = readdir(dir);
        }
        closedir(dir);
    }
    if (!success) {
        success = rmdir(name);
    }
}

int remove_directory(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;
    if (d) {
        struct dirent *p;
        r = 0;
        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;
            len = path_len + strlen(p->d_name) + 2; 
            buf = malloc(len);
            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                    r2 = remove_directory(buf);
                    else
                    r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }
    if (!r){
        r = rmdir(path);
    } 
    return r;
}

void make_dummy_file(const char* name, uint32_t start, uint32_t end) {
    FILE *fp = fopen(name, "w");
    if (fp) {
        for (uint32_t i = start; i < end; i++) {
            fprintf(fp, "%d", i);
        }
        fclose(fp);
    }
}
#endif // TEST_UTIL_H
