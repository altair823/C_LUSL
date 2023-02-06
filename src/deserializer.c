#include "deserializer.h"

bool mkdir_recursive(const char *path) {
    if (path == NULL) {
        return false;
    }
    char *path_copy = (char *) malloc(sizeof(char *) * (strlen(path) + 1));
    memcpy(path_copy, path, strlen(path) + 1);
    if (path_copy[strlen(path_copy) - 1] == '/') {
        path_copy[strlen(path_copy) - 1] = '\0';
    }
    char *path_ptr = path_copy;
    while (*path_ptr != '\0') {
        if (*path_ptr == '/') {
            *path_ptr = '\0';
            if (MKDIR(path_copy, 0777) == -1 && errno != EEXIST) {
                free(path_copy);
                DEBUG_MSG("Cannot create directory!");
                return false;
            }
            *path_ptr = '/';
        }
        path_ptr++;
    }
    free(path_copy);
    return true;
}

bool deserialize(deserializer_t *deserializer) {
    if (deserializer == NULL 
    || deserializer->input_file == NULL 
    || deserializer->output_dir == NULL) {
        return false;
    }
    FILE *input_file = fopen(deserializer->input_file, "rb");
    if (input_file == NULL) {
        DEBUG_MSG("Cannot open input file!");
        return false;
    }
    INIT_BUFREADER(input_file_reader, input_file, DESER_BUFFER_SIZE);
    INIT_FILE_HEADER(header);
    if (!read_header(&header, &input_file_reader)) {
        DEBUG_MSG("Cannot read header!");
        return false;
    }
    if (cmp_version(header.version) == VERSION_INCOMPATIBLE) {
        DEBUG_MSG("Version mismatch!");
        return false;
    }
    if (header.is_encrypted) {
        DEBUG_MSG("Encrypted file is not supported!");
        return false;
    }
    if (header.is_compressed) {
        DEBUG_MSG("Compressed file is not supported!");
        return false;
    }
    for (size_t i = 0; i < header.file_count; i++) {
        INIT_META(metadata);
        // Read metadata
        if (!deser_br_meta(&input_file_reader, &metadata)) {
            DEBUG_MSG("Cannot read metadata!");
            return false;
        }
        for (size_t j = 0; j < metadata.path_length; j++) {
            #ifdef WIN32
            if (metadata.path[j] == '/') {
                metadata.path[j] = '\\';
            }
            #else
            if (metadata.path[j] == '\\') {
                metadata.path[j] = '/';
            }
            #endif
        }
        // Create directory
        char *output_path = (char *) malloc(sizeof(char *) * (strlen(deserializer->output_dir) + 
        strlen(metadata.path) + 2));
        sprintf(output_path, "%s/%s", deserializer->output_dir, metadata.path);
        if (!mkdir_recursive(output_path)) {
            DEBUG_MSG("Cannot create directory!");
            return false;
        }
        // Read file data and write to file.
        FILE *output_file = fopen(output_path, "wb");
        if (output_file == NULL) {
            DEBUG_MSG("Cannot open output file!");
            return false;
        }
        for (size_t counter = 0; counter < metadata.size;) {
            INIT_BINARY(binary);
            size_t buf_read_size = (metadata.size - counter) > DESER_BUFFER_SIZE ? DESER_BUFFER_SIZE : (metadata.size - counter);
            if (!read_bufreader(&input_file_reader, &binary, buf_read_size)) {
                DEBUG_MSG("Cannot read binary!");
                return false;
            }
            fwrite(binary.data, sizeof(char), buf_read_size, output_file);
            counter += binary.length;
            FREE_BINARY(binary);
        }
        fclose(output_file);

        if (!verify_hash(metadata.hash, output_path)) {
            DEBUG_MSG("Hash mismatch!");
            return false;
        }
        free(output_path);
        #ifdef VERBOSE
        printf("Deserialize Complete! : File %zu / %zu: %s\n", i, header.file_count, metadata.path);
        #endif
        FREE_META(metadata);
    }
    return true;
}

bool read_header(fheader_t *header, bufreader_t *input_file_reader) {
    if (header == NULL || input_file_reader == NULL) {
        return false;
    }
    if (!deser_br_fheader(input_file_reader, header)) {
        DEBUG_MSG("Cannot read header!");
        return false;
    }
    return true;    
}