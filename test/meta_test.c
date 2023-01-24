#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "meta.h"

bool fhash_test(){
    char *filename = "test/meta_test1.bin";
    /* Using fhash function. */
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    FILE *file2 = fopen(filename, "rb");
    assert(file2 != NULL);
    uint8_t *hash = fhash(file2);
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", hash[i]);
    }
    printf("\n");
    fclose(file2);

    /* Compare actual hash of file. */
    FILE *file3 = fopen(filename, "rb");
    assert(file3 != NULL);
    sha3_context ctx;
    INIT_SHA3(&ctx);
    unsigned char data[100];
    size_t n;
    while ((n = fread(data, 1, sizeof(unsigned char) * 100, file3)) > 0) {
        sha3_Update(&ctx, data, n);
    }
    const uint8_t *hash2 = sha3_Finalize(&ctx);

    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", hash2[i]);
    }

    printf("\n");
    fclose(file3);

    assert(memcmp(hash, hash2, 32) == 0);
    remove(filename);
    return true;
}

bool fflags_test(){
    char *filename = "test/meta_test2.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    FILE *file2 = fopen(filename, "rb");
    assert(file2 != NULL);
    uint8_t flags = fflags(file2);
    // print all bits
    for (int i = 0; i < 8; i++) {
        printf("%d", (flags >> i) & 1);
    }
    fclose(file2);
    remove(filename);
}

int main () {
    fhash_test();
    fflags_test();
    return 0;
}