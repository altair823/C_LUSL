#ifndef F_HASH_H
#define F_HASH_H

#include "sha3.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

#include "binary.h"

#define BUFFER_SIZE 1024
#define SHA3_256
// #define SHA3_512

#define NULL_HASH '0'

#ifdef SHA3_256
#define HASH_SIZE 32
#define INIT_SHA3(x) sha3_Init256(x)
#endif
#ifdef SHA3_512
#define HASH_SIZE 64
#define INIT_SHA3(x) sha3_Init512(x)
#endif

#define FILL_NULL_HASH(x) for (int i = 0; i < HASH_SIZE; i++) { x[i] = NULL_HASH; }

#define IS_DIR 0x80
#define IS_FILE 0x40
#define IS_LINK 0x20

typedef struct{
    unsigned char*path;
    unsigned int path_length;
    uint8_t hash[HASH_SIZE];
    bool is_file;
    bool is_dir;
    bool is_link;
    uint64_t size;
} meta_t;

/**
 * @brief Get the hash of a file.
 * @param file File to get hash of.
 * @return Hash of file. Must be freed.
*/
void fhash(FILE *file, uint8_t *hash);

/**
 * @brief Get the flags of a file.
 * @param file File to get flags of.
 * @details Flags are indicate types of the file. For example, if file is a directory, then flag IS_DIR will be set.
 * @return Flags of file.
*/
uint8_t fflags(FILE *file);

/**
 * @brief Get the size of a file.
 * @param file File to get size of.
 * @return Size of file.
*/
uint64_t fsize(FILE *file);

/**
 * @brief Get the metadata of a file.
 * @param filename File to get metadata of.
 * @return Metadata of file.
*/
bool fmeta(char *filename, meta_t *meta);

bool meta_serialize(meta_t *meta, binary_t *bin);

#endif