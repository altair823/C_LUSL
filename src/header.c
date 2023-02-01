#include "header.h"

bool ser_flabel(binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    binary->length = sizeof(FILE_LABEL) - 1;
    binary->data = (uint8_t *)malloc(binary->length);
    memcpy(binary->data, FILE_LABEL, binary->length);
    return true;
}

enum v_cmp_result cmp_version(version_t file_version) {
    if (file_version.major != MAJOR_VERSION) {
        return VERSION_INCOMPATIBLE;
    } else {
        if (file_version.minor > MINOR_VERSION) {
            return VERSION_INCOMPATIBLE;
        }
    }
    return VERSION_COMPATIBLE;
}

void ser_version(version_t version, binary_t *binary) {
    binary->length = 3;
    binary->data = malloc(binary->length);
    binary->data[0] = version.major;
    binary->data[1] = version.minor;
    binary->data[2] = version.patch;
}

bool ser_fheader(fheader_t *header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);

    INIT_BINARY(file_label_binary);
    ser_flabel(&file_label_binary);
    concat_binary(binary, &file_label_binary);

    byte_t version_start_offset[1] = {VERSION_START_OFFSET};
    append_binary(binary, version_start_offset, 1);

    INIT_BINARY(version_bin);
    ser_version(header->version, &version_bin);
    concat_binary(binary, &version_bin);

    INIT_BINARY(flags_binary);
    ser_fflags(header, &flags_binary);
    concat_binary(binary, &flags_binary);

    INIT_BINARY(file_count_binary);
    ser_fcount(header, &file_count_binary);
    concat_binary(binary, &file_count_binary);
    return true;
}

bool ser_fflags(fheader_t *header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    binary->length = 1;
    binary->data = (uint8_t *)malloc(binary->length);
    binary->data[0] = 0;
    if (header->is_encrypted) {
        binary->data[0] |= ENCRYPTED_FLAG;
    }
    if (header->is_compressed) {
        binary->data[0] |= COMPRESSED_FLAG;
    }
    return true;
}

bool ser_fcount(fheader_t *header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    INIT_BINARY(file_count_bytes);
    uint64_to_le_arr(header->file_count, &file_count_bytes);
    binary->length = file_count_bytes.length + 1;
    binary->data = (uint8_t *)malloc(binary->length);
    if (file_count_bytes.length > 0xFF) {
        assert(false && "File count is too large");
        return false;
    }
    binary->data[0] = (uint8_t)file_count_bytes.length;
    memcpy(binary->data + 1, file_count_bytes.data, file_count_bytes.length);
    return true;
}

bool deser_bin_fheader(binary_t *binary, fheader_t *new_header) {
    CHECK_BINARY_PTR_NOT_NULL(binary);
    if (memcmp(binary->data, FILE_LABEL, sizeof(FILE_LABEL) - 1) != 0) {
        return false;
    }
    size_t offset = sizeof(FILE_LABEL) - 1;
    if (binary->data[offset] != VERSION_START_OFFSET) {
        return false;
    }
    offset++;
    EMPTY_VERSION(version);
    version.major = binary->data[offset++];
    version.minor = binary->data[offset++];
    version.patch = binary->data[offset++];
    if (cmp_version(version) == VERSION_INCOMPATIBLE) {
        return false;
    }
    new_header->version = version;
    new_header->is_encrypted = binary->data[offset] & ENCRYPTED_FLAG;
    new_header->is_compressed = binary->data[offset] & COMPRESSED_FLAG;
    offset++;
    uint64_t file_count = 0;
    uint8_t file_count_bytes = binary->data[offset++];
    for (int i = 0; i < file_count_bytes; i++) {
        file_count |= (uint64_t)binary->data[offset++] << (i * 8);
    }
    new_header->file_count = file_count;
    return true;
}

bool deser_br_fheader(bufreader_t *reader, fheader_t *new_header) {
    CHECK_BUFREADER_PTR_NOT_NULL(reader);
    INIT_BINARY(label_binary);
    size_t label_length = sizeof(FILE_LABEL) - 1;
    read_bufreader(reader, &label_binary, label_length);
    if (memcmp(label_binary.data, FILE_LABEL, sizeof(FILE_LABEL) - 1) != 0) {
        return false;
    }
    FREE_BINARY(label_binary);
    INIT_BINARY(version_binary);
    read_bufreader(reader, &version_binary, 4);
    if (version_binary.data[0] != VERSION_START_OFFSET) {
        return false;
    }
    EMPTY_VERSION(version);
    version.major = version_binary.data[1];
    version.minor = version_binary.data[2];
    version.patch = version_binary.data[3];
    if (cmp_version(version) == VERSION_INCOMPATIBLE) {
        return false;
    }
    new_header->version = version;
    FREE_BINARY(version_binary);
    INIT_BINARY(flags_binary);
    read_bufreader(reader, &flags_binary, 1);
    new_header->is_encrypted = flags_binary.data[0] & ENCRYPTED_FLAG;
    new_header->is_compressed = flags_binary.data[0] & COMPRESSED_FLAG;
    FREE_BINARY(flags_binary);
    INIT_BINARY(file_count_binary);
    read_bufreader(reader, &file_count_binary, 1);
    uint64_t file_count = 0;
    uint8_t file_count_bytes = file_count_binary.data[0];
    FREE_BINARY(file_count_binary);
    INIT_BINARY(file_count_bytes_binary);
    read_bufreader(reader, &file_count_bytes_binary, file_count_bytes);
    for (int i = 0; i < file_count_bytes; i++) {
        file_count |= (uint64_t)file_count_bytes_binary.data[i] << (i * 8);
    }
    new_header->file_count = file_count;
    FREE_BINARY(file_count_bytes_binary);
    return true;
}