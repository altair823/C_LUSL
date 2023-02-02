#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ftw.h>

#ifndef _WIN32
#include <dirent.h>
#elif _WIN32
#include "dirent_win.h"
#endif

#include "header.h"
#include "meta.h"
#include "bufread.h"

typedef struct _stack {
    char **data;
    size_t size;
    size_t top;
} path_stack_t;
#define INIT_STACK(x) path_stack_t x = {NULL, 0, 0}; \
MALLOC_STACK(x)

#define MALLOC_STACK(x) x.data = malloc(DEFAULT_STACK_SIZE * sizeof(char *)); \
x.size = DEFAULT_STACK_SIZE; \
x.top = 0

#define DEFAULT_STACK_SIZE 100

#define PUSH_STACK(stack, value) if (stack.top == stack.size) \
{ stack.size += DEFAULT_STACK_SIZE; \
stack.data = realloc(stack.data, stack.size * sizeof(char *)); } \
stack.data[stack.top++] = value

#define POP_STACK(stack, value) if (stack.top == 0) { assert(false && "Pop from empty stack!"); } \
else { value = stack.data[--stack.top]; }

#define FREE_STACK(stack) for (size_t i = 0; i < stack.top; i++) { free(stack.data[i]); } \
free(stack.data); stack.data = NULL; stack.size = 0; stack.top = 0

/**
 * @brief Get the file list of a directory recursively.
 * @param root_dir Directory to get file list of.
 * @param file_list List of files.
 * @param file_count Number of files.
 * @return True if successful, false otherwise.
*/
bool get_file_list(char *root_dir, path_stack_t *file_list);

/**
 * @brief The serializer struct
 * @details The serializer struct is used to serialize root directory.
*/
typedef struct {
    char *root_dir; ///< Root directory to serialize.
    char *output_file; ///< Output file path.
    uint8_t option; ///< Option of the serializer.
    path_stack_t *file_list; ///< List of files to serialize.
} serializer_t;

#define DEFAULT_FILE_LIST_SIZE 100

#define INIT_SERIALIZER(x, root_dir, output_file_path) serializer_t x = \
{root_dir, output_file_path, 0, NULL}; \
x.file_list = malloc(sizeof(path_stack_t)); \
MALLOC_STACK((*x.file_list)) ///< Initialize a serializer.

#define FREE_SERIALIZER(x) FREE_STACK((*x.file_list)); \
free(x.file_list); \
x.root_dir = NULL; \
x.output_file = NULL; \
x.option = 0; \
x.file_list = NULL; ///< Free a serializer.

#define SET_OPTION(x, new_option) x.option = new_option ///< Set the option of a serializer.

/**
 * @brief Serialize a directory.
 * @param serializer Serializer to use.
 * @return True if successful, false otherwise.
*/
bool serialize(serializer_t *serializer);

/**
 * @brief Write the LUSL header of a file.
 * @param output_file Output file.
 * @param fheader Header to write.
 * @return True if successful, false otherwise.
*/
bool write_fheader(FILE *output_file, fheader_t *fheader);

/**
 * @brief Write the metadata of a file.
 * @param output_file Output file.
 * @param fmeta Metadata to write.
 * @return True if successful, false otherwise.
*/
bool write_fmeta(FILE *output_file, meta_t *fmeta);

/**
 * @brief Write the data of a file.
 * @param output_file Output file.
 * @param input_file_reader Reader of the input file.
 * @param fsize Size of the file.
 * @return True if successful, false otherwise.
*/
bool write_fdata(FILE *output_file, bufreader_t *input_file_reader, size_t fsize);

#endif // SERIALIZER_H