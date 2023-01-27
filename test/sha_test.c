#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "test_util.h"
#include "sha3.h"
#include "meta.h"

void string_hash_test() {
    INIT_TEST_SUITE;

    int size = 32;

    sha3_context c;
    sha3_Init256(&c);
    sha3_Update(&c, "abc", 3);
    const uint8_t *hash = sha3_Finalize(&c);

    sha3_context c2;
    sha3_Init256(&c2);
    sha3_Update(&c2, "abc", 3);
    const uint8_t *hash2 = sha3_Finalize(&c2);

    assert(memcmp(hash, hash2, size) == 0);
    END_TEST_SUITE;
}

void file_hash_test() {
    INIT_TEST_SUITE;
    FILE *file = fopen("test/test.bin", "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    FILE *file2 = fopen("test/test.bin", "rb");
    assert(file2 != NULL);
    sha3_context ctx;
    
    sha3_Init256(&ctx);

    unsigned char data[10];
    size_t n;
    while ((n = fread(data, 1, sizeof(unsigned char) * 10, file2)) > 0) {
        sha3_Update(&ctx, data, n);
    }
    const uint8_t *hash = sha3_Finalize(&ctx);
    fclose(file2);

    FILE *file3 = fopen("test/test.bin", "rb");
    assert(file3 != NULL);
    sha3_context ctx2;
    sha3_Init256(&ctx2);
    unsigned char data2[100];
    size_t n2;
    while ((n2 = fread(data2, 1, sizeof(unsigned char) * 100, file3)) > 0) {
        sha3_Update(&ctx2, data2, n2);
    }
    const uint8_t *hash2 = sha3_Finalize(&ctx2);
    fclose(file3);

    assert(memcmp(hash, hash2, HASH_SIZE) == 0);

    remove("test/test.bin");
    END_TEST_SUITE;
}

int main() {
    string_hash_test();
    file_hash_test();

    return 0;
}