#include <stdio.h>
#include <assert.h>

#include "config.h"
#include "header.h"

void version_test() {
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
}

void file_header_test() {
    INIT_FILE_HEADER(header);
    assert(header.version.major == MAJOR_VERSION);
    assert(header.version.minor == MINOR_VERSION);
    assert(header.version.patch == PATCH_VERSION);
    assert(header.is_encrypted == false);
    assert(header.is_compressed == false);
    assert(header.file_count == 0);
}

int main () {
    version_test();
    return 0;
}