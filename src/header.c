#include "header.h"

void version_to_byte(version_t version, binary_t *byte) {
    byte->length = 3;
    byte->data = malloc(byte->length);
    byte->data[0] = version.major;
    byte->data[1] = version.minor;
    byte->data[2] = version.patch;
}