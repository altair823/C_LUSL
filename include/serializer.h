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

typedef struct _stack {
    char **data;
    size_t size;
    size_t top;
} stack_t;
#define INIT_STACK(x) stack_t x = {NULL, 0, 0}; \
x.data = malloc(DEFAULT_STACK_SIZE * sizeof(char *)); \
x.size = DEFAULT_STACK_SIZE; \
x.top = 0 ///< Initialize a stack.
#define DEFAULT_STACK_SIZE 100 ///< Default stack size.

#define PUSH_STACK(stack, value) if (stack.top == stack.size) \
{ stack.size += DEFAULT_STACK_SIZE; \
stack.data = realloc(stack.data, stack.size * sizeof(char *)); } \
stack.data[stack.top++] = value ///< Push a value onto the stack.

#define POP_STACK(stack, value) if (stack.top == 0) { assert(false && "Pop from empty stack!"); } \
else { value = stack.data[--stack.top]; } ///< Pop a value off the stack.

#define FREE_STACK(stack) for (size_t i = 0; i < stack.top; i++) { free(stack.data[i]); } \
free(stack.data); stack.data = NULL; stack.size = 0; stack.top = 0 ///< Free a stack.

typedef struct {
    char *root_dir;
    uint8_t option;
    stack_t *file_list;
} serializer_t;

#define INIT_SERIALIZER(x) serializer_t x = {NULL, 0, NULL}

#define DEFAULT_FILE_LIST_SIZE 100

/**
 * @brief Get the file list of a directory recursively.
 * @param root_dir Directory to get file list of.
 * @param file_list List of files.
 * @param file_count Number of files.
 * @return True if successful, false otherwise.
*/
bool get_file_list(char *root_dir, stack_t *file_list);

#endif // SERIALIZER_H