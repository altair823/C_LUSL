#ifndef F_HASH_H
#define F_HASH_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include "stat_win.h"
#include "dirent_win.h"
#define MKDIR(path, mode) _mkdir(path)
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path, mode) mkdir(path, mode)
#endif

#include "config.h"
#include "sha3.h"
#include "md5.h"
#include "binary.h"
#include "bufread.h"

#define HASH_BUFFER_SIZE 1024
#define MD5 ///< MD5 hash.
// #define SHA3_256 ///< SHA-3 256-bit hash.
// #define SHA3_512 ///< SHA-3 512-bit hash.

#define NULL_HASH '0' ///< Null hash value.

#ifdef MD5
#define HASH_SIZE 16 ///< Size of hash.
#define INIT_CONTEXT(context) MD5Context context ///< Initialize MD5.
#define INIT_HASH(context_ptr) md5Init(context_ptr) ///< Initialize MD5.
#define UPDATE_HASH(context_ptr, input, len) md5Update(context_ptr, input, len) ///< Update MD5.
#define FINALIZE_HASH(context_ptr, result) md5Finalize(context_ptr); result = (*context_ptr).digest ///< Finalize MD5.
#endif
#ifdef SHA3_256
#define HASH_SIZE 32 ///< Size of hash.
#define INIT_CONTEXT(context) sha3_context context ///< Initialize SHA-3.
#define INIT_HASH(context_ptr) sha3_Init256(context_ptr) ///< Initialize SHA-3.
#define UPDATE_HASH(context_ptr, input, len) sha3_Update(context_ptr, input, len) ///< Update SHA-3.
#define FINALIZE_HASH(context_ptr, result) result = sha3_Finalize(context_ptr) ///< Finalize SHA-3.

#endif
#ifdef SHA3_512
#define HASH_SIZE 64 ///< Size of hash.
#define INIT_CONTEXT(context) sha3_context context ///< Initialize SHA-3.
#define INIT_HASH(context_ptr) sha3_Init512(context_ptr) ///< Initialize SHA-3.
#define UPDATE_HASH(context_ptr, input, len) sha3_Update(context_ptr, input, len) ///< Update SHA-3.
#define FINALIZE_HASH(context_ptr, result) result = sha3_Finalize(context_ptr) ///< Finalize SHA-3.
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
    char *path; ///< Path of file.
    uint16_t path_length; ///< Length of path.
    uint8_t hash[HASH_SIZE]; ///< Hash of file.
    bool is_file; ///< Flag whether target is file.
    bool is_dir; ///< Flag whether target is directory.
    bool is_link; ///< Flag whether target is link.
    uint64_t size; ///< Size of file.
} meta_t;

#define INIT_META(x) meta_t x = { NULL, 0, { NULL_HASH, }, false, false, false, 0 } ///< Initialize metadata.
#define FREE_META(x) free(x.path); ///< Free metadata.

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
bool ser_meta(meta_t *meta, binary_t *bin);

/**
 * @brief Deserialize metadata.
 * @param meta Metadata to deserialize to.
 * @param bin Binary to deserialize from.
 * @return True if successful, false otherwise.
*/
bool deser_bin_meta(binary_t *bin, meta_t *meta);

/**
 * @brief Serialize metadata.
 * @param reader Buffer reader to serialize metadata from.
 * @param meta Metadata to serialize.
 * @return True if successful, false otherwise.
*/
bool deser_br_meta(bufreader_t *reader, meta_t *meta);

/**
 * @brief Verify hash of file.
 * @param original_hash Original hash of file which is written on metadata.
 * @param new_filename File to verify hash of.
 * @return True if hash is correct, false otherwise.
*/
bool verify_hash(uint8_t *original_hash, char *new_filename);

#endif