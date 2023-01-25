#include <stdio.h>
#include <binary.h>
#include <assert.h>

int main () {
    BINARY_INIT(binary);
    uint64_to_byte_arr(89213500000, &binary);
    // Print all bits in the binary array.
    for (int i = 0; i < binary.length; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", (binary.data[i] >> (7 - j)) & 1);
        }
        printf(" ");
    }
    assert(binary.length == 5);
    assert(binary.data[0] == 0b01100000);
    assert(binary.data[1] == 0b11111010);
    assert(binary.data[2] == 0b10001001);
    assert(binary.data[3] == 0b11000101);
    assert(binary.data[4] == 0b00010100);
    BINARY_FREE(binary);
}