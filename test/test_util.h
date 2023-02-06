#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include "stat_win.h"
#define MKDIR(path, mode) _mkdir(path)
#include "dirent_win.h"
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path, mode) mkdir(path, mode)
#endif



#define INIT_TEST_SUITE printf("%s...", __func__)
#define END_TEST_SUITE printf("OK\n")

void make_directory(const char* name) {
    DIR *dir = opendir(name);
    if (dir) {
        closedir(dir);
        return;
    } else {
        if (MKDIR(name, 0777) == -1) {
            printf("Cannot create dir %s\n", name);\
            return;
        }
        else {
            return;
        }
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
            buf = (char *)malloc(len);
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
