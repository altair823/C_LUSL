#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#define MKDIR(path, mode) _mkdir(path)
#include "dirent_win.h"
#elif _WIN32
#include <dirent.h>
#define MKDIR(path, mode) mkdir(path, mode)
#endif

#include "config.h"
#include "header.h"
#include "meta.h"
#include "bufread.h"

/**
 * @brief Create parent directory recursively.
 * @param path Path to create.
 * @return True if successful, false otherwise.
 * @note If the directory already exists, this function will return true.
 * @note If the directory cannot be created, this function will return false.
 * @details This function will create the parent directory of the given path. 
 * The last directory in the path considered to be a file, and will not be created.
*/
bool mkdir_recursive(const char *path);

#define DESER_BUFFER_SIZE 8192 ///< Buffer size for deserialization.

typedef struct {
    char *input_file;
    char *output_dir;
    uint8_t option;
    uint64_t file_count;
    bufreader_t *input_file_reader;
} deserializer_t;

#define INIT_DESERIALIZER(x, input_file, output_dir) deserializer_t x = \
{input_file, output_dir, 0, 0, NULL}

#define FREE_DESERIALIZER(x) if (x.input_file_reader != NULL) { \
if (x.input_file_reader->chunk.data) { free(x.input_file_reader->chunk.data);\
x.input_file_reader->chunk.data = NULL; x.input_file_reader->chunk.length = 0; } \
x.input_file_reader = NULL; \
}

/**
 * @brief Deserialize a file.
 * @param deserializer Deserializer to use.
 * @return True if successful, false otherwise.
 */
bool deserialize(deserializer_t *deserializer);

bool read_header(fheader_t *header, bufreader_t *input_file_reader);

#endif // DESERIALIZER_H