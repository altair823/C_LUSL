#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include <stdint.h>

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
} Version;

/**
 * @brief The FileHeader struct
*/
typedef struct {
    Version version; ///< Version of file.
    bool is_encrypted; ///< Flag whether file is encrypted.
    bool is_compressed; ///< Flag whether file is compressed.
    uint64_t file_count; ///< Number of files in file.
} FileHeader;


#endif