#include <stdio.h>
#include <assert.h>

#include "test_util.h"
#include "config.h"
#include "header.h"

void file_header_test() {
    INIT_TEST_SUITE;
    INIT_FILE_HEADER(header);
    assert(header.version.major == MAJOR_VERSION);
    assert(header.version.minor == MINOR_VERSION);
    assert(header.version.patch == PATCH_VERSION);
    assert(header.is_encrypted == false);
    assert(header.is_compressed == false);
    assert(header.file_count == 0);
    END_TEST_SUITE;
}

void serialize_file_label_test() {
    INIT_TEST_SUITE;
    INIT_BINARY(binary);
    serialize_file_label(&binary);
    assert(binary.length == sizeof(FILE_LABEL) - 1); // -1 to exclude null terminator
    for (int i = 0; i < binary.length; i++) {
        assert(binary.data[i] == FILE_LABEL[i]);
    }
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void version_init_test() {
    INIT_TEST_SUITE;

    EMPTY_VERSION(empty_version);
    assert(empty_version.major == 0);
    assert(empty_version.minor == 0);
    assert(empty_version.patch == 0);

    unsigned char major = MAJOR_VERSION;
    unsigned char minor = MINOR_VERSION;
    unsigned char patch = PATCH_VERSION;

    CURRENT_VERSION(version);
    assert(version.major == major);
    assert(version.minor == minor);
    assert(version.patch == patch);

    END_TEST_SUITE;
}

void serialize_version_test() {
    INIT_TEST_SUITE;
    CURRENT_VERSION(version);
    binary_t binary;
    serialize_version(version, &binary);
    assert(binary.length == 3);
    assert(binary.data[0] == version.major);
    assert(binary.data[1] == version.minor);
    assert(binary.data[2] == version.patch);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void serialize_flags_test() {
    INIT_TEST_SUITE;
    INIT_FILE_HEADER(header);
    header.is_encrypted = true;
    header.is_compressed = true;
    INIT_BINARY(binary);
    serialize_flags(header, &binary);
    assert(binary.data[0] == ENCRYPTED_FLAG | COMPRESSED_FLAG);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void serialize_file_count_test() {
    INIT_TEST_SUITE;
    INIT_FILE_HEADER(header);
    header.file_count = 0x123456789ABCDEF0;
    INIT_BINARY(binary);
    serialize_file_count(header, &binary);
    assert(binary.length == 9);
    assert(binary.data[0] == 8); // 8 bytes
    // little endian encoding of 0x123456789ABCDEF0
    assert(binary.data[1] == 0xF0); 
    assert(binary.data[2] == 0xDE);
    assert(binary.data[3] == 0xBC);
    assert(binary.data[4] == 0x9A);
    assert(binary.data[5] == 0x78);
    assert(binary.data[6] == 0x56);
    assert(binary.data[7] == 0x34);
    assert(binary.data[8] == 0x12);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void serialize_file_header_test() {
    INIT_TEST_SUITE;
    INIT_FILE_HEADER(header);
    header.is_encrypted = true; 
    header.is_compressed = true; // Flags are 1 byte.
    header.file_count = 0x123456789ABCDEF0; // 9 bytes after serialized.
    INIT_BINARY(binary);
    serialize_file_header(header, &binary);
    // -1 to exclude null terminator in FILE_LABEL 
    // and 4 bytes for version.
    assert(binary.length == sizeof(FILE_LABEL) - 1 + 14); 
    size_t i = 0;
    for (; i < sizeof(FILE_LABEL) - 1; i++) {
        assert(binary.data[i] == FILE_LABEL[i]);
    }
    assert(binary.data[i++] == VERSION_START_OFFSET);
    assert(binary.data[i++] == header.version.major);
    assert(binary.data[i++] == header.version.minor);
    assert(binary.data[i++] == header.version.patch);

    assert(flag_is_set(binary.data[i], ENCRYPTED_FLAG));
    assert(flag_is_set(binary.data[i++], COMPRESSED_FLAG));

    assert(binary.data[i++] == 8); // 8 bytes
    // little endian encoding of 0x123456789ABCDEF0
    assert(binary.data[i++] == 0xF0);
    assert(binary.data[i++] == 0xDE);
    assert(binary.data[i++] == 0xBC);
    assert(binary.data[i++] == 0x9A);
    assert(binary.data[i++] == 0x78);
    assert(binary.data[i++] == 0x56);
    assert(binary.data[i++] == 0x34);
    assert(binary.data[i++] == 0x12);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

void deserialize_file_header_test() {
    INIT_TEST_SUITE;
    INIT_FILE_HEADER(header);
    header.is_encrypted = true;
    header.is_compressed = true;
    header.file_count = 0x123456789ABCDEF0;
    INIT_BINARY(binary);
    serialize_file_header(header, &binary);
    INIT_FILE_HEADER(deserialized_header);
    deserialize_file_header(&binary, &deserialized_header);
    assert(deserialized_header.version.major == header.version.major);
    assert(deserialized_header.version.minor == header.version.minor);
    assert(deserialized_header.version.patch == header.version.patch);
    assert(deserialized_header.is_encrypted == header.is_encrypted);
    assert(deserialized_header.is_compressed == header.is_compressed);
    assert(deserialized_header.file_count == header.file_count);
    FREE_BINARY(binary);
    END_TEST_SUITE;
}

int main () {
    file_header_test();
    serialize_file_label_test();
    version_init_test();
    serialize_version_test();
    serialize_flags_test();
    serialize_file_count_test();
    serialize_file_header_test();
    deserialize_file_header_test();
    return 0;
}