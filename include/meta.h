#ifndef F_HASH_H
#define F_HASH_H

#include "sha3.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define SHA_256

#ifdef SHA_256
#define HASH_SIZE 32
#define INIT_SHA3(x) sha3_Init256(x)
#endif
#ifdef SHA_512
#define HASH_SIZE 64
#define INIT_SHA3(x) sha3_Init512(x)
#endif

#define IS_DIR 0x80
#define IS_FILE 0x40
#define IS_LINK 0x20

typedef struct{
    unsigned *path;
    unsigned int path_length;
    unsigned *hash;
    bool is_file;
    bool is_dir;
    bool is_symlink;
} meta_t;


uint8_t *fhash(FILE *file);

uint8_t fflags(FILE *file);

meta_t *fmeta(char *filename);

#endif