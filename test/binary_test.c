#include <stdio.h>
#include <binary.h>
#include <assert.h>

#include "test_util.h"

void concat_binary_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary1);
    binary1.length = 10;
    binary1.data = (uint8_t *)malloc(binary1.length);
    for (int i = 0; i < binary1.length; i++) {
        binary1.data[i] = i;
    }
    assert(binary1.length == 10);
    for (int i = 0; i < binary1.length; i++) {
        assert(binary1.data[i] == i);
    }

    INIT_BINARY(binary2);
    binary2.length = 12;
    binary2.data = (uint8_t *)malloc(binary2.length);
    for (int i = 0; i < binary2.length; i++) {
        binary2.data[i] = i + 10;
    }
    concat_binary(&binary1, &binary2);
    assert(binary1.length == 22);
    for (int i = 0; i < binary1.length; i++) {
        assert(binary1.data[i] == i);
    }
    FREE_BINARY(binary1);
    FREE_BINARY(binary2);
    END_TEST_SUITE;
}

void append_binary_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary1);
    binary1.length = 10;
    binary1.data = (uint8_t *)malloc(binary1.length);
    for (int i = 0; i < binary1.length; i++) {
        binary1.data[i] = i;
    }
    assert(binary1.length == 10);
    for (int i = 0; i < binary1.length; i++) {
        assert(binary1.data[i] == i);
    }

    size_t before_length = binary1.length;
    byte_t to_append[5] = {0, 1, 2, 3, 4};
    append_binary(&binary1, to_append, 5);
    assert(binary1.length == 15);
    for (int i = 0; i < 5; i++) {
        assert(binary1.data[i + before_length] == i);
    }

    FREE_BINARY(binary1);
    END_TEST_SUITE;
}

void create_binary_ref_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary);
    binary.length = 10;
    binary.data = (uint8_t *)malloc(binary.length);
    for (int i = 0; i < binary.length; i++) {
        binary.data[i] = i;
    }
    assert(binary.length == 10);
    for (int i = 0; i < binary.length; i++) {
        assert(binary.data[i] == i);
    }

    INIT_BINARY(ref1);
    create_binary_ref(&binary, &ref1, 5, 5);
    assert(ref1.length == 5);
    for (int i = 0; i < ref1.length; i++) {
        assert(ref1.data[i] == i + 5);
    }
    assert(ref1.is_ref == true);
    assert(binary.is_ref_exist == true);
    assert(binary.ref_size == 1);
    assert(binary.ref_list != NULL);
    assert(binary.ref_list[0] == &ref1);

    INIT_BINARY(ref2);
    create_binary_ref(&binary, &ref2, 0, 5);
    assert(ref2.length == 5);
    for (int i = 0; i < ref2.length; i++) {
        assert(ref2.data[i] == i);
    }
    assert(ref2.is_ref == true);
    assert(binary.is_ref_exist == true);
    assert(binary.ref_size == 2);
    assert(binary.ref_list != NULL);
    assert(binary.ref_list[0] == &ref1);
    assert(binary.ref_list[1] == &ref2);

    FREE_BINARY(ref1);
    assert(ref1.data != NULL);
    assert(binary.data != NULL);
    assert(binary.is_ref_exist == true);
    assert(binary.ref_size == 2);
    assert(binary.ref_list != NULL);
    assert(binary.ref_list[0] == &ref1);
    assert(binary.ref_list[1] == &ref2);

    FREE_BINARY(binary);
    assert(ref1.data == NULL);
    assert(ref1.is_ref == false);
    assert(ref2.data == NULL);
    assert(ref2.is_ref == false);
    assert(binary.data == NULL);
    assert(binary.is_ref_exist == false);
    assert(binary.ref_size == 0);
    assert(binary.ref_list == NULL);
    END_TEST_SUITE;
}

void uint64_to_le_arr_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary);
    uint64_to_le_arr(89213500000, &binary);
    
    assert(binary.length == 5);
    assert(binary.data[0] == 0b01100000);
    assert(binary.data[1] == 0b11111010);
    assert(binary.data[2] == 0b10001001);
    assert(binary.data[3] == 0b11000101);
    assert(binary.data[4] == 0b00010100);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void le_arr_to_uint64_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary);
    uint64_to_le_arr(89213500000, &binary);
    uint64_t num = le_arr_to_uint64(&binary);
    assert(num == 89213500000);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

int main () {
    concat_binary_test();
    append_binary_test();
    create_binary_ref_test();
    uint64_to_le_arr_test();
    le_arr_to_uint64_test();
    return 0;
}