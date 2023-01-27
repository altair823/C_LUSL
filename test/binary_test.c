#include <stdio.h>
#include <binary.h>
#include <assert.h>

#include "test_util.h"

void uint64_to_le_arr_test() {
    INIT_TEST_SUITE;
    BINARY_INIT(binary);
    uint64_to_le_arr(89213500000, &binary);
    
    assert(binary.length == 5);
    assert(binary.data[0] == 0b01100000);
    assert(binary.data[1] == 0b11111010);
    assert(binary.data[2] == 0b10001001);
    assert(binary.data[3] == 0b11000101);
    assert(binary.data[4] == 0b00010100);
    BINARY_FREE(binary);
    END_TEST_SUITE;
}

void le_arr_to_uint64_test() {
    INIT_TEST_SUITE;
    BINARY_INIT(binary);
    uint64_to_le_arr(89213500000, &binary);
    uint64_t num = le_arr_to_uint64(&binary);
    assert(num == 89213500000);
    BINARY_FREE(binary);
    END_TEST_SUITE;
}

int main () {
    uint64_to_le_arr_test();
    le_arr_to_uint64_test();
    return 0;
}