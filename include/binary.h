#ifndef BINARY_H
#define BINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte_t;

typedef struct {
    byte_t *data;
    size_t length;
} binary_t;

#define BINARY_INIT(x) binary_t x = { NULL, 0 }
#define BINARY_FREE(x) if (x.data) { free(x.data); x.data = NULL; x.length = 0; } else { x.length = 0; }

bool uint64_to_le_arr(uint64_t num, binary_t *result);

uint64_t le_arr_to_uint64(binary_t *arr);

bool flag_is_set(byte_t flag, byte_t flag_to_check);

#endif