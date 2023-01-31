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

bool ser_meta(meta_t *meta, binary_t *bin) {
    CHECK_BINARY_PTR_NULL(bin)
    else if (meta->path_length > 0xFFFF) {
        printf("Path is too long. (>65535)\n");
        return false; // Path is too long.
    }
    
    
    byte_t path_length_bytes[2];
    path_length_bytes[0] = (meta->path_length >> 8) & 0xFF;
    path_length_bytes[1] = meta->path_length & 0xFF;

    INIT_BINARY(size_bytes);
    uint64_to_le_arr(meta->size, &size_bytes);


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

    FREE_BINARY(size_bytes);
    return true;
}

bool deser_bin_meta(binary_t *bin, meta_t *meta) {
    CHECK_BINARY_PTR_NOT_NULL(bin);

    // Deserialize path length.
    uint16_t path_length = (bin->data[0] << 8) | bin->data[1];

    // Deserialize path.
    meta->path = (char *) malloc(sizeof(char) * (path_length + 1));
    memcpy(meta->path, bin->data + 2, path_length);
    meta->path[path_length] = '\0';
    meta->path_length = path_length;

    // Deserialize flags.
    uint8_t flags = bin->data[2 + path_length];
    meta->is_file = flags & IS_FILE;
    meta->is_dir = flags & IS_DIR;
    meta->is_link = flags & IS_LINK;

    // Deserialize file size.
    uint8_t size_bytes_length = flags & 0x0F;
    INIT_BINARY(size_bytes);
    size_bytes.length = size_bytes_length;
    size_bytes.data = (byte_t *) malloc(sizeof(byte_t) * size_bytes_length);
    memcpy(size_bytes.data, bin->data + 3 + path_length, size_bytes_length);
    meta->size = le_arr_to_uint64(&size_bytes);
    FREE_BINARY(size_bytes);

    // Deserialize hash.
    memcpy(meta->hash, bin->data + 3 + path_length + size_bytes_length, HASH_SIZE);

    return true;
}

bool deser_br_meta(bufreader_t *reader, meta_t *meta) {
    CHECK_BUFREADER_PTR_NOT_NULL(reader);

    // Deserialize path length.
    INIT_BINARY(path_length_bytes);
    read_bufreader(reader, &path_length_bytes, 2);
    uint16_t path_length = (path_length_bytes.data[0] << 8) | 
        path_length_bytes.data[1];
    FREE_BINARY(path_length_bytes);

    // Deserialize path.
    INIT_BINARY(path_bin);
    read_bufreader(reader, &path_bin, path_length);
    meta->path = (char *) malloc(sizeof(char) * (path_length + 1));
    memcpy(meta->path, path_bin.data, path_length);
    meta->path[path_length] = '\0';
    meta->path_length = path_length;
    FREE_BINARY(path_bin);

    // Deserialize flags.
    INIT_BINARY(flags_bin);
    read_bufreader(reader, &flags_bin, 1);
    uint8_t flags = flags_bin.data[0];
    meta->is_file = flags & IS_FILE;
    meta->is_dir = flags & IS_DIR;
    meta->is_link = flags & IS_LINK;
    FREE_BINARY(flags_bin);

    // Deserialize file size.
    INIT_BINARY(size_bytes_bin);
    uint8_t size_bytes_length = flags & 0x0F;
    read_bufreader(reader, &size_bytes_bin, size_bytes_length);
    INIT_BINARY(size_bytes);
    size_bytes.length = size_bytes_length;
    size_bytes.data = (byte_t *) malloc(sizeof(byte_t) * size_bytes_length);
    memcpy(size_bytes.data, size_bytes_bin.data, size_bytes_length);
    meta->size = le_arr_to_uint64(&size_bytes);
    FREE_BINARY(size_bytes);
    FREE_BINARY(size_bytes_bin);

    // Deserialize hash.
    INIT_BINARY(hash_bin);
    read_bufreader(reader, &hash_bin, HASH_SIZE);
    memcpy(meta->hash, hash_bin.data, HASH_SIZE);
    FREE_BINARY(hash_bin);

    return true;
}