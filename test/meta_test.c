#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "meta.h"

void fhash_test() {
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
    uint8_t hash[HASH_SIZE];
    fhash(file2, hash);
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
}

void fflags_test() {
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
    printf("flags: ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (flags >> i) & 1);
    }
    printf("\n");

    // check if flags are correct
    assert(flags & IS_FILE);
    fclose(file2);
    remove(filename);
}

void fmeta_test() {
    char *filename = "test/meta_test3.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    meta_t meta;
    fmeta(filename, &meta);
    printf("path: %s\n", meta.path);
    printf("size: %ld\n", meta.size);
    printf("is dir: %d\n", meta.is_dir);
    printf("is file: %d\n", meta.is_file);
    printf("is link: %d\n", meta.is_link);
    printf("hash: ");
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%02x ", meta.hash[i]);
    }
    printf("\n");
    remove(filename);
}

int main () {
    #ifdef SHA3_256
    printf("SHA3_256\n");
    #endif
    #ifdef SHA3_512
    printf("SHA3_512\n");
    #endif
    fhash_test();
    fflags_test();
    fmeta_test();

    return 0;
}