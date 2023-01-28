#include "header.h"

bool serialize_file_label(binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    binary->length = sizeof(FILE_LABEL) - 1;
    binary->data = (uint8_t *)malloc(binary->length);
    memcpy(binary->data, FILE_LABEL, binary->length);
    return true;
}

enum v_cmp_result compare_version(version_t file_version) {
    if (file_version.major != MAJOR_VERSION) {
        return VERSION_INCOMPATIBLE;
    } else {
        if (file_version.minor > MINOR_VERSION) {
            return VERSION_INCOMPATIBLE;
        }
    }
    return VERSION_COMPATIBLE;
}

void serialize_version(version_t version, binary_t *binary) {
    binary->length = 3;
    binary->data = malloc(binary->length);
    binary->data[0] = version.major;
    binary->data[1] = version.minor;
    binary->data[2] = version.patch;
}

bool serialize_file_header(file_header_t header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);

    INIT_BINARY(file_label_binary);
    serialize_file_label(&file_label_binary);
    concat_binary(binary, &file_label_binary);

    byte_t version_start_offset[1] = {VERSION_START_OFFSET};
    append_binary(binary, version_start_offset, 1);

    INIT_BINARY(version_bin);
    serialize_version(header.version, &version_bin);
    concat_binary(binary, &version_bin);

    INIT_BINARY(flags_binary);
    serialize_flags(header, &flags_binary);
    concat_binary(binary, &flags_binary);

    INIT_BINARY(file_count_binary);
    serialize_file_count(header, &file_count_binary);
    concat_binary(binary, &file_count_binary);
    return true;
}

bool serialize_flags(file_header_t header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    binary->length = 1;
    binary->data = (uint8_t *)malloc(binary->length);
    binary->data[0] = 0;
    if (header.is_encrypted) {
        binary->data[0] |= ENCRYPTED_FLAG;
    }
    if (header.is_compressed) {
        binary->data[0] |= COMPRESSED_FLAG;
    }
    return true;
}

bool serialize_file_count(file_header_t header, binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    INIT_BINARY(file_count_bytes);
    uint64_to_le_arr(header.file_count, &file_count_bytes);
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

bool deserialize_file_header(binary_t *binary, file_header_t *new_header) {
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
    if (compare_version(version) == VERSION_INCOMPATIBLE) {
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