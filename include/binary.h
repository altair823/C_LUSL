#ifndef BINARY_H
#define BINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t byte_t;

/**
 * @brief The binary struct
 * @details The binary struct is used to store binary data.
 * The data is stored in a byte array.
*/
typedef struct {
    byte_t *data; /**< Data of binary. */
    size_t length; /**< Length of data. */
} binary_t;

#define BINARY_INIT(x) binary_t x = { NULL, 0 } /**< Initialize binary. */
#define BINARY_FREE(x) if (x.data) { free(x.data); x.data = NULL; x.length = 0; } else { x.length = 0; } /**< Free binary. */

/**
 * @brief Convert uint64_t to little endian array.
 * @param num Number to convert.
 * @param result Resulting array.
 * @return True if successful, false otherwise.
 * @details The resulting array will be dynamically allocated. 
 * The caller is responsible for freeing the array.
 * @see le_arr_to_uint64
*/
bool uint64_to_le_arr(uint64_t num, binary_t *result);

/**
 * @brief Convert little endian array to uint64_t.
 * @param arr binary_t value to convert.
 * @return Converted number.
 * @see uint64_to_le_arr
*/
uint64_t le_arr_to_uint64(binary_t *arr);

/**
 * @brief Check if flag is set.
 * @param flag Flag to check.
 * @param flag_to_check Flag to check for.
 * @return True if flag is set, false otherwise.
*/
bool flag_is_set(byte_t flag, byte_t flag_to_check);

#endif