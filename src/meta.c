#include "meta.h"

uint8_t *fhash(FILE *file) {
    if (file){
        sha3_context ctx;

        INIT_SHA3(&ctx);

        unsigned char data[BUFFER_SIZE];
        size_t n;
        while ((n = fread(data, 1, sizeof(unsigned char) * BUFFER_SIZE, file)) > 0) {
            sha3_Update(&ctx, data, n);
        }
        const uint8_t *hash = sha3_Finalize(&ctx);
        return (unsigned char *) hash;
    } else {
        return NULL;
    }
}

uint8_t fflags(FILE *file) {
    if (file){
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