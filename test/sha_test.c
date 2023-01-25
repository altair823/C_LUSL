#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sha3.h"
#include "meta.h"

void string_hash_test() {

    int size = 32;

    sha3_context c;
    sha3_Init256(&c);
    sha3_Update(&c, "abc", 3);
    const uint8_t *hash = sha3_Finalize(&c);

    for (int i = 0; i < size; i++) {
        if (i > 0) {
            printf("%02x ", hash[i]);
        }
    }
    printf("\n");

    sha3_context c2;
    sha3_Init256(&c2);
    sha3_Update(&c2, "abc", 3);
    const uint8_t *hash2 = sha3_Finalize(&c2);

    for (int i = 0; i < size; i++) {
        if (i > 0) {
            printf("%02x ", hash2[i]);
        }
    }

    for (int i = 0; i < size; i++){
        assert(hash[i] == hash2[i]);
    }
    printf("\n");
}

void file_hash_test() {
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

    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", hash[i]);
    }

    printf("\n");
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

    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", hash2[i]);
    }

    printf("\n");
    fclose(file3);

    assert(memcmp(hash, hash2, HASH_SIZE) == 0);

    remove("test/test.bin");
}

int main() {
    string_hash_test();
    file_hash_test();

    return 0;
}