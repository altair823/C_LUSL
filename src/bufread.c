#include <bufread.h>

bool read_chunk(FILE *file, chunk_t *chunk, size_t chunk_size) {
    if (!file) {
        DEBUG_MSG("File is NULL");
        return false;
    }
    CHECK_CHUNK_PTR_NULL(chunk);
    chunk->length = chunk_size;
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
bool read_bufreader(bufreader_t *bufreader, binary_t *binary, size_t length) {
    CHECK_BINARY_PTR_NULL(binary);
    if (!bufreader) {
        DEBUG_MSG("bufreader is NULL");
        return false;
    }
    if (!bufreader->file) {
        DEBUG_MSG("File in bufreader is NULL");
        return false;
    }
    // if there is no data in chunk, read new chunk from file.
    if (!bufreader->chunk.data) {
        if (!read_chunk(bufreader->file, &bufreader->chunk, bufreader->chunk_size)) {
            DEBUG_MSG("Failed to read chunk");
            return false;
        }
        bufreader->current_offset = 0;
    }
    // if length is less or equal than the length of remaining data in chunk, 
    // and write it to the binary data.
    if (bufreader->current_offset + length <= bufreader->chunk.length) {    
        binary->length = length;
        binary->data = (unsigned char *) malloc(sizeof(unsigned char) * binary->length);
        memcpy(binary->data, bufreader->chunk.data + bufreader->current_offset, length);
        bufreader->current_offset += length;
    } 
    // if length is greater than the length of remaining data in chunk,
    // read new chunk from file and write it to the binary data.
    else {
        binary->length = length;
        binary->data = (unsigned char *) malloc(sizeof(unsigned char) * binary->length);
        size_t counter = 0;
        memcpy(binary->data, bufreader->chunk.data + bufreader->current_offset, bufreader->chunk.length - bufreader->current_offset);
        counter += bufreader->chunk.length - bufreader->current_offset;
        while (counter + bufreader->chunk.length < length) {
            FREE_CHUNK(bufreader->chunk);
            if (!read_chunk(bufreader->file, &bufreader->chunk, bufreader->chunk_size)) {
                DEBUG_MSG("Failed to read chunk");
                return false;
            }
            memcpy(binary->data + counter, bufreader->chunk.data, bufreader->chunk.length);
            counter += bufreader->chunk.length;
        }
        size_t new_offset = length - counter;
        FREE_CHUNK(bufreader->chunk);
        if (!read_chunk(bufreader->file, &bufreader->chunk, bufreader->chunk_size)) {
            DEBUG_MSG("Failed to read chunk");
            return false;
        }
        memcpy(binary->data + counter, bufreader->chunk.data, new_offset);
        bufreader->current_offset = new_offset;
    }

    return true;
}