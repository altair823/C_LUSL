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
        return 0x00;
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
        meta->path_length = strlen(filename);
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

bool meta_serialize(meta_t *meta, binary_t *bin) {
    if (bin->data != NULL) {
        printf("Binary is already occupied.\n");
        return false; // Binary is already occupied.
    } else if (meta->path_length > 0xFFFF) {
        printf("Path is too long. (>65535)\n");
        return false; // Path is too long.
    }
    
    
    byte_t path_length_bytes[2];
    path_length_bytes[0] = (meta->path_length >> 8) & 0xFF;
    path_length_bytes[1] = meta->path_length & 0xFF;

    BINARY_INIT(size_bytes);
    uint64_to_byte_arr(meta->size, &size_bytes);


    bin->length = 2 // Path length binary size
                + meta->path_length // Path
                + 1 // Flags
                + size_bytes.length // Size
                + HASH_SIZE; // Hash

    bin->data = (byte_t *) malloc(sizeof(byte_t) * bin->length);

    // Serialize path.
    bin->data[0] = path_length_bytes[0];
    bin->data[1] = path_length_bytes[1];
    memcpy(bin->data + 2, meta->path, meta->path_length);

    // Serialize flags.
    uint8_t flags = 0;
    if (meta->is_file) {
        flags |= IS_FILE;
    } else if (meta->is_dir) {
        flags |= IS_DIR;
    } else if (meta->is_link) {
        flags |= IS_LINK;
    }
    bin->data[2 + meta->path_length] = flags;

    // Serialize file size.
    bin->data[2 + meta->path_length] += size_bytes.length;
    memcpy(bin->data + 3 + meta->path_length, size_bytes.data, size_bytes.length);
    
    // Serialize hash.
    memcpy(bin->data + 3 + meta->path_length + size_bytes.length, meta->hash, HASH_SIZE);

    BINARY_FREE(size_bytes);
    return true;
}