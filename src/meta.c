#include "meta.h"

void fhash(FILE *file, uint8_t *hash) {
    if (file) {
        sha3_context ctx;

        INIT_SHA3(&ctx);

        unsigned char data[BUFFER_SIZE];
        size_t n;
        while ((n = fread(data, 1, sizeof(unsigned char) * BUFFER_SIZE, file)) > 0) {
            sha3_Update(&ctx, data, n);
        }
        const uint8_t *src_hash = sha3_Finalize(&ctx);
        memcpy(hash, src_hash, HASH_SIZE);
    } else {
        FILL_NULL_HASH(hash);
    }
}

uint8_t fflags(FILE *file) {
    if (file) {
        struct stat st;
        fstat(fileno(file), &st);
        uint8_t flags = 0;
        if (S_ISDIR(st.st_mode)) {
            flags |= IS_DIR;
        } else if (S_ISREG(st.st_mode)) {
            flags |= IS_FILE;
        } else if (S_ISLNK(st.st_mode)) {
            flags |= IS_LINK;
        }
        return flags;
    } else {
        return 0x01; // Error flag.
    }
}

uint64_t fsize(FILE *file) {
    if (file) {
        struct stat st;
        fstat(fileno(file), &st);
        return st.st_size;
    } else {
        return 0;
    }
}

bool fmeta(char *filename, meta_t *meta) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        meta->path = (char *) malloc(sizeof(char *) * (strlen(filename) + 1));
        memcpy(meta->path, filename, strlen(filename) + 1);
        fhash(file, meta->hash);
        uint8_t flags = fflags(file);
        meta->size = fsize(file);
        fclose(file);
        meta->is_file = flags & IS_FILE;
        meta->is_dir = flags & IS_DIR;
        meta->is_link = flags & IS_LINK;
        return true;
    } else {
        return false;
    }
}