#include <stdio.h>
#include <assert.h>

#include "test_util.h"
#include "config.h"
#include "header.h"

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

void version_to_byte_test() {
    INIT_TEST_SUITE;
    CURRENT_VERSION(version);
    binary_t byte;
    version_to_byte(version, &byte);
    assert(byte.length == 3);
    assert(byte.data[0] == version.major);
    assert(byte.data[1] == version.minor);
    assert(byte.data[2] == version.patch);
    END_TEST_SUITE;
}

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

int main () {
    version_init_test();
    version_to_byte_test();
    file_header_test();
    return 0;
}