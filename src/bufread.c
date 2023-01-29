#include <bufread.h>

bool read_chunk(FILE *file, chunk_t *chunk) {
    if (!file) {
        return false;
    }
    CHECK_CHUNK_PTR_NULL(chunk);
    chunk->data = (unsigned char *) malloc(sizeof(unsigned char) * chunk->length);
    size_t read_count = fread(chunk->data, sizeof(unsigned char), chunk->length, file);
    if (read_count < chunk->length) {
        if (ferror(file)) {
            free(chunk->data);
            chunk->data = NULL;
            return false;
        } else {
            chunk->length = read_count;
            // The rest of the chunk is filled with 0s.
            memset(chunk->data + read_count, 0, chunk->length - read_count);
            return true;
        }
    } else {
        return true;
    }
}