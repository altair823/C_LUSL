#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "test_util.h"
#include "meta.h"

void fhash_test() {
    INIT_TEST_SUITE;
    char *filename = "test/fhash_test.bin";
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
    fclose(file3);

    assert(memcmp(hash, hash2, 32) == 0);
    remove(filename);
    END_TEST_SUITE;
}

void fflags_test() {
    INIT_TEST_SUITE;
    char *filename = "test/fflags_test.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    FILE *file2 = fopen(filename, "rb");
    assert(file2 != NULL);
    uint8_t flags = fflags(file2);

    // check if flags are correct
    assert(flags & IS_FILE);
    fclose(file2);
    remove(filename);
    END_TEST_SUITE;
}

void fmeta_test() {
    INIT_TEST_SUITE;
    char *filename = "test/fmeta_test.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    meta_t meta;
    fmeta(filename, &meta);
    assert(strcmp(meta.path, filename) == 0);
    struct stat st;
    fstat(fileno(file), &st);
    assert(meta.size == st.st_size);
    assert(meta.is_dir == false);
    assert(meta.is_file == true);
    assert(meta.is_link == false);
    uint8_t hash[HASH_SIZE];
    fhash(file, hash);
    assert(memcmp(meta.hash, hash, HASH_SIZE) == 0);

    fclose(file);
    remove(filename);
    FREE_META(meta);
    END_TEST_SUITE;
}

void serialize_test() {
    INIT_TEST_SUITE;
    char *filename = "test/serialize_test.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    meta_t meta;
    fmeta(filename, &meta);
    INIT_BINARY(binary);
    serialize_meta(&meta, &binary);
    
    // check if serialized data is correct
    // Chech path
    uint16_t path_size = 0;
    path_size = binary.data[0] << 8;
    path_size += binary.data[1];
    assert(path_size == meta.path_length);
    unsigned char *path = malloc(path_size);
    memcpy(path, &binary.data[2], path_size);
    assert(memcmp(path, meta.path, path_size) == 0);

    // Check flags
    uint8_t flags = binary.data[2 + path_size];
    assert(flag_is_set(flags, IS_DIR) == meta.is_dir);
    assert(flag_is_set(flags, IS_FILE) == meta.is_file);
    assert(flag_is_set(flags, IS_LINK) == meta.is_link);

    // Check size
    uint8_t size_bytes = binary.data[2 + path_size] & 0xf;
    uint64_t size = 0;
    for (int i = 0; i < size_bytes; i++) {
        size += binary.data[3 + path_size + i] << 8 * i;
    }
    assert(size == meta.size);

    // Check hash
    uint8_t hash[HASH_SIZE];
    memcpy(hash, &binary.data[3 + path_size + size_bytes], HASH_SIZE);
    assert(memcmp(hash, meta.hash, HASH_SIZE) == 0);
    
    // Free memory
    FREE_BINARY(binary);
    remove(filename);
    FREE_META(meta);
    END_TEST_SUITE;
}

void deserialize_test() {
    INIT_TEST_SUITE;
    char *filename = "test/deserialize_test.bin";
    FILE *file = fopen(filename, "wb");
    assert(file != NULL);
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "%d ", i);
    }
    fclose(file);
    meta_t meta;
    fmeta(filename, &meta);
    INIT_BINARY(binary);
    serialize_meta(&meta, &binary);
    meta_t meta2;
    deserialize_meta(&meta2, &binary);
    assert(strcmp(meta.path, meta2.path) == 0);
    assert(meta.size == meta2.size);
    assert(meta.is_dir == meta2.is_dir);
    assert(meta.is_file == meta2.is_file);
    assert(meta.is_link == meta2.is_link);
    assert(memcmp(meta.hash, meta2.hash, HASH_SIZE) == 0);
    FREE_BINARY(binary);
    remove(filename);

    FREE_META(meta);
    FREE_META(meta2);
    END_TEST_SUITE;
}

int main () {
    fhash_test();
    fflags_test();
    fmeta_test();
    serialize_test();
    deserialize_test();
    return 0;
}