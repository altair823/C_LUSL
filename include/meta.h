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
#define SHA3_256 ///< SHA-3 256-bit hash.
// #define SHA3_512 ///< SHA-3 512-bit hash.

#define NULL_HASH '0' ///< Null hash value.

#ifdef SHA3_256
#define HASH_SIZE 32 ///< Size of hash.
#define INIT_SHA3(x) sha3_Init256(x) ///< Initialize SHA-3.
#endif
#ifdef SHA3_512
#define HASH_SIZE 64 ///< Size of hash.
#define INIT_SHA3(x) sha3_Init512(x) ///< Initialize SHA-3.
#endif

#define FILL_NULL_HASH(x) for (int i = 0; i < HASH_SIZE; i++) { x[i] = NULL_HASH; } ///< Fill hash with null hash value.

#define IS_DIR 0x80
#define IS_FILE 0x40
#define IS_LINK 0x20

/**
 * @brief The metadata struct
 * @details The metadata struct is used to store the metadata of a file.
*/
typedef struct{
    unsigned char *path; /**< Path of file. */
    uint16_t path_length; /**< Length of path. */
    uint8_t hash[HASH_SIZE]; /**< Hash of file. */
    bool is_file; /**< Flag whether target is file. */
    bool is_dir; /**< Flag whether target is directory. */
    bool is_link; /**< Flag whether target is link. */
    uint64_t size; /**< Size of file. */
} meta_t;

/**
 * @brief Get the hash of a file.
 * @details Hash is calculated using SHA-3.
 * @param file File to get hash of.
 * @param hash Hash of file.
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
 * @param meta Metadata of file.
 * @return True if successful, false otherwise.
*/
bool fmeta(char *filename, meta_t *meta);

/**
 * @brief Serialize metadata.
 * @param meta Metadata to serialize.
 * @param bin Binary to serialize metadata to.
 * @return True if successful, false otherwise.
*/
bool serialize_meta(meta_t *meta, binary_t *bin);

/**
 * @brief Deserialize metadata.
 * @param meta Metadata to deserialize to.
 * @param bin Binary to deserialize from.
 * @return True if successful, false otherwise.
*/
bool deserialize_meta(meta_t *meta, binary_t *bin);

#define FREE_META(x) free(x.path); ///< Free metadata.

#endif