#include "header.h"

bool serialize_file_label(binary_t *binary) {
    CHECK_BINARY_PTR_NULL(binary);
    binary->length = sizeof(FILE_LABEL) - 1;
    binary->data = (uint8_t *)malloc(binary->length);
    memcpy(binary->data, FILE_LABEL, binary->length);
    return true;
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
        assert_m(false, "File count is too large");
    }
    binary->data[0] = (uint8_t)file_count_bytes.length;
    memcpy(binary->data + 1, file_count_bytes.data, file_count_bytes.length);
    return true;
}