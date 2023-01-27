#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

/// File header label.
#define FILE_LABEL LUSL Serialized File
/// Offset for version in file header.
#define VERSION_START_OFFSET 0x01
#define ENCRYPTED_FLAG 0x80
#define COMPRESSED_FLAG 0x40

/**
 * @brief The Version struct
 * @details The version struct is used to store the version of the file.
 * The major version is the first byte, the minor version is the second byte,
 * and the patch version is the third byte.
*/
typedef struct {
    unsigned char major;
    unsigned char minor;
    unsigned char patch;
} version_t;

///< Create version struct from current major, minor, and patch versions of program.
#define CURRENT_VERSION(version) version_t version = {MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION}

///< Create empty version struct.
#define EMPTY_VERSION(version) version_t version = {0, 0, 0}

/**
 * @brief Convert version struct to byte.
 * @param version Version struct to convert.
 * @return Byte representation of version struct.
*/
uint8_t version_to_byte(version_t version);

/**
 * @brief The file header struct
*/
typedef struct {
    version_t version; ///< Version of file.
    bool is_encrypted; ///< Flag whether file is encrypted.
    bool is_compressed; ///< Flag whether file is compressed.
    uint64_t file_count; ///< Number of files in file.
} file_header_t;

///< Create file header struct with current version of program.
#define INIT_FILE_HEADER(header) file_header_t header = {{MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION}, false, false, 0}

#endif