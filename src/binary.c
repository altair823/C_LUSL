#include "binary.h"

bool uint64_to_le_arr(uint64_t num, binary_t *result) {
    if (result->data != NULL){
        return false;
    }
    uint8_t temp[8];
    // Split the 64-bit number into 8 8-bit numbers in big-endian order.
    for (int i = 0; i < 8; i++) {
        temp[i] = (num >> ((7 - i) * 8)) & 0xFF;
    }
    int index = 0;
    for (int i = 0; i < 8; i++) {
        if (temp[i] == 0) {
            index++;
        } else {
            break;
        }
    }
    if (index > 8) {
        return false;
    }
    uint8_t length = 8 - index;
    result->length = length;
    result->data = (uint8_t *) malloc(sizeof(uint8_t) * length);
    
    // Save the meaningful numbers to result array in little-endian order.
    for (int i = 0; i < length; i++) {
        result->data[i] = temp[7 - i];
    }
    return true;
}

uint64_t le_arr_to_uint64(binary_t *arr) {
    uint64_t result = 0;
    for (int i = 0; i < arr->length; i++) {
        result |= (uint64_t) arr->data[i] << (i * 8);
    }
    return result;
}

bool flag_is_set(byte_t flag, byte_t flag_to_check) {
    return (flag & flag_to_check) == flag_to_check;
}