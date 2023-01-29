#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

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
 * @param chunk Chunk to read.
 * @param chunk_size Size of chunk.
 * @return True if successful, false otherwise.
 */
bool read_chunk(FILE *file, chunk_t *chunk);