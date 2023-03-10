#include "serializer.h"

bool get_file_list(char *root_dir, path_stack_t *file_list) {
    if (root_dir == NULL || file_list == NULL) {
        return false;
    }
    INIT_STACK(dir_stack);
    PUSH_STACK(dir_stack, root_dir);
    while (dir_stack.top > 0) {
        char *subroot_dir = NULL;
        POP_STACK(dir_stack, subroot_dir);
        if (subroot_dir) {
            DIR *dir = opendir(subroot_dir);
            if (dir) {
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_DIR) {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                            char *path;
                            path = (char *) malloc(sizeof(char *) * (strlen(subroot_dir) + strlen(entry->d_name) + 2));
                            memcpy(path, subroot_dir, strlen(subroot_dir));
                            printf("%s\n", path);
                        #ifdef _WIN32
                            path[strlen(subroot_dir)] = '\\';
                        #else
                            path[strlen(subroot_dir)] = '/';
                        #endif
                            memcpy(path + strlen(subroot_dir) + 1, entry->d_name, strlen(entry->d_name) + 1);
                            PUSH_STACK(dir_stack, path);
                        }
                    } else {
                        char *filename = (char *) malloc(sizeof(char *) * (strlen(subroot_dir) + strlen(entry->d_name) + 2));
                        memcpy(filename, subroot_dir, strlen(subroot_dir));
                    #ifdef _WIN32
                        filename[strlen(subroot_dir)] = '\\';
                    #else
                        filename[strlen(subroot_dir)] = '/';
                    #endif
                        memcpy(filename + strlen(subroot_dir) + 1, entry->d_name, strlen(entry->d_name) + 1);
                        PUSH_STACK((*file_list), filename);
                    }
                }
                closedir(dir);
            }
        }
        if (subroot_dir && subroot_dir != root_dir) {
            free(subroot_dir);
        }
    }
    FREE_STACK(dir_stack);
    return true;
}

bool serialize(serializer_t *serializer) {
    if (serializer == NULL) {
        return false;
    }
    if (serializer->file_list == NULL) {
        DEBUG_MSG("file_list is not initialized!");
        return false;
    }
    if (isDir(serializer->root_dir) == false) {
        DEBUG_MSG("root_dir is not a directory!");
        return false;
    }
    if (!get_file_list(serializer->root_dir, serializer->file_list)) {
        DEBUG_MSG("get_file_list failed!");
        return false;
    }
    FILE *output_file = fopen(serializer->output_file, "wb");
    if (output_file == NULL) {
        DEBUG_MSG("Cannot open output file!");
        return false;
    }
    // Write file header
    INIT_FILE_HEADER(header);
    header.is_compressed = serializer->option & COMPRESSED_FLAG;
    header.is_encrypted = serializer->option & ENCRYPTED_FLAG;
    header.file_count = serializer->file_list->top;
    
    if (!write_fheader(output_file, &header)) {
        DEBUG_MSG("write_fheader failed!");
        return false;
    }

    for (size_t i = 0; i < serializer->file_list->top; i++) {
        char *filename = serializer->file_list->data[i];

        // Write metadata of file
        INIT_META(metadata);
        fmeta(filename, &metadata);
        if (write_fmeta(output_file, &metadata) == false) {
            DEBUG_MSG("write_fmeta failed!");
            return false;
        }

        // Write file data
        FILE *input_file = fopen(filename, "rb");
        INIT_BUFREADER(input_file_reader, input_file, SER_BUFFER_SIZE);
        if (write_fdata(output_file, &input_file_reader, metadata.size) == false) {
            DEBUG_MSG("write_fdata failed!");
            return false;
        }
        FREE_BUFREADER(input_file_reader);
        fclose(input_file);
        #ifdef VERBOSE
        printf("File %s is serialized.\n", filename);
        #endif
        
        FREE_META(metadata);
    }
    fclose(output_file);
    return true;
}

bool write_fheader(FILE *output_file, fheader_t *fheader) {
    INIT_BINARY(header_binary);
    if (!ser_fheader(fheader, &header_binary)) {
        DEBUG_MSG("ser_fheader failed!");
        return false;
    }
    if (fwrite(header_binary.data, sizeof(char), header_binary.length, output_file) 
    != header_binary.length) {
        DEBUG_MSG("fwrite failed!");
        return false;
    }
    FREE_BINARY(header_binary);
    return true;
}

bool write_fmeta(FILE *output_file, meta_t *fmeta) {
    INIT_BINARY(meta_binary);
    if (!ser_meta(fmeta, &meta_binary)) {
        DEBUG_MSG("ser_fmeta failed!");
        return false;
    }
    if (fwrite(meta_binary.data, sizeof(char), meta_binary.length, output_file) 
    != meta_binary.length) {
        DEBUG_MSG("fwrite failed!");
        return false;
    }
    FREE_BINARY(meta_binary);
    return true;
}

bool write_fdata(FILE *output_file, bufreader_t *input_file_reader, size_t fsize) {
    size_t buffer_size = input_file_reader->chunk_size;
    while (fsize > buffer_size) {
        INIT_BINARY(data_binary);
        if (!read_bufreader(input_file_reader, &data_binary, buffer_size)) {
            DEBUG_MSG("read_bufreader failed!");
            return false;
        }
        if (fwrite(data_binary.data, sizeof(char), buffer_size, output_file) != buffer_size) {
            DEBUG_MSG("fwrite failed!");
            return false;
        }
        fsize -= buffer_size;
        FREE_BINARY(data_binary);
    }
    if (fsize > 0) {
        INIT_BINARY(data_binary);
        if (!read_bufreader(input_file_reader, &data_binary, fsize)) {
            DEBUG_MSG("read_bufreader failed!");
            return false;
        }
        if (fwrite(data_binary.data, sizeof(char), fsize, output_file) != fsize) {
            DEBUG_MSG("fwrite failed!");
            return false;
        }
        FREE_BINARY(data_binary);
    }
    return true;
}

int isDir(const char* fileName) {
    struct stat path;
    stat(fileName, &path);
    return S_ISDIR(path.st_mode);
}