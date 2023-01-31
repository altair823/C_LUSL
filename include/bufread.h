#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "binary.h"

/**
 * @brief The chunk struct using for reading file in chunks. 
 */
typedef struct _chunk_t {
    unsigned char *data; ///< Data of chunk.
    size_t length; ///< Length of data.
} chunk_t;

#define INIT_CHUNK(x) chunk_t x = { NULL, 0 } ///< Initialize chunk.
#define FREE_CHUNK(x) if (x.data) { free(x.data); x.data = NULL; x.length = 0; } ///< Free chunk_t.
#define CHECK_CHUNK_PTR_NOT_NULL(x) if (!x->data) \
{ assert(false && "Data in chunk_t is NULL"); return false; } ///< Check if chunk pointer is null.
#define CHECK_CHUNK_PTR_NULL(x) if (x->data) \
{ assert(false && "Data in chunk_t is not NULL"); return false; } ///< Check if chunk pointer is not null.

/**
 * @brief Read file in chunks.
 * @param file File to read.
 * @param chunk \ref chunk_t to read.
 * @param chunk_size Size of chunk.
 * @return True if successful, false otherwise.
 */
bool read_chunk(FILE *file, chunk_t *chunk, size_t chunk_size);

/**
 * @brief The bufreader struct using for reading file in chunks. 
 * @details This struct is used for reading file in chunks.
 * The file variable must be closed by the user.
 */
typedef struct _bufreader_t {
    FILE *file; ///< File to read.
    chunk_t chunk; ///< \ref chunk_t to read.
    size_t chunk_size; ///< Size of chunk.
    size_t current_offset; ///< Current offset in chunk.
} bufreader_t;

#define INIT_BUFREADER(x, file, buf_size) bufreader_t x = { file, { NULL, buf_size }, buf_size, 0 } ///< Initialize bufreader.
#define FREE_BUFREADER(x) if (x.chunk.data) { free(x.chunk.data); x.chunk.data = NULL; x.chunk.length = 0; } ///< Free bufreader_t. Not close file.
#define CHECK_BUFREADER_PTR_NOT_NULL(x) if (!x->file) \
{ assert(false && "Data in bufreader_t is NULL"); return false; } ///< Check if bufreader pointer is null.
#define CHECK_BUFREADER_PTR_NULL(x) if (x->file) \
{ assert(false && "Data in bufreader_t is not NULL"); return false; } ///< Check if bufreader pointer is not null.

/**
 * @brief Read binary data of the given length. 
 * @param bufreader \ref bufreader_t to read.
 * @param binary \ref binary_t to read.
 * @param length Length of binary.
 * @return True if successful, false otherwise.
 */
bool read_bufreader(bufreader_t *bufreader, binary_t *binary, size_t length);