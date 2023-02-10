#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"

#include "config.h"
#include "binary.h"
#include "bufread.h"

/// File header label.
#define FILE_LABEL "LUSL Serialized File"

/**
 * @brief Serialize file label to byte array.
 * @param binary Byte array to store result in.
 * @return True if successful, false otherwise.
 * @details The file label is stored as a non-null terminated string.
*/
bool ser_flabel(binary_t *binary);

#define VERSION_START_OFFSET 0x01 ///< Offset of version in file header.
#define ENCRYPTED_FLAG 0x80 ///< Flag whether file is encrypted.
#define COMPRESSED_FLAG 0x40 ///< Flag whether file is compressed.

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


#define CURRENT_VERSION(version) version_t version = {MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION} ///< Create version struct from current major, minor, and patch versions of program.

#define EMPTY_VERSION(version) version_t version = {0, 0, 0} ///< Create empty version struct.

/**
 * @brief The result of comparing two version structs.
*/
enum v_cmp_result {
    VERSION_COMPATIBLE = 1, ///< The version of the file is compatible but maybe older than the current version of the program.
    VERSION_INCOMPATIBLE = 0 ///< The version of the file is not compatible.
};

/**
 * @brief Compare two version structs.
 * @param file_version Version of file.
 * @return Version compatibility.
*/
enum v_cmp_result cmp_version(version_t file_version);

/**
 * @brief Convert version struct to byte.
 * @param version Version struct to convert.
 * @param byte Byte to store result in.
*/
void ser_version(version_t version, binary_t *byte);

/**
 * @brief The file header struct
*/
typedef struct {
    version_t version; ///< Version of file.
    bool is_encrypted; ///< Flag whether file is encrypted.
    bool is_compressed; ///< Flag whether file is compressed.
    uint64_t file_count; ///< Number of files in file.
} fheader_t;

///< Create file header struct with current version of program.
#define INIT_FILE_HEADER(header) fheader_t header = {{MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION}, false, false, 0}

/**
 * @brief Serialize file header to byte array.
 * @param header File header to serialize.
 * @param binary Byte array to store result in.
 * @return True if successful, false otherwise.
*/
bool ser_fheader(fheader_t *header, binary_t *binary);

/**
 * @brief Serialize flags to byte array.
 * @param header File header to serialize.
 * @param binary Byte array to store result in.
 * @return True if successful, false otherwise.
*/
bool ser_fflags(fheader_t *header, binary_t *binary);

/**
 * @brief Serialize file count to byte array.
 * @param header File header to serialize.
 * @param binary Byte array to store result in.
 * @return True if successful, false otherwise.
 * @details The file count is stored as variable length byte array with little 
 * endian encoding. The first byte is the length of the byte array, and 
 * the remaining bytes are the file count.
*/
bool ser_fcount(fheader_t *header, binary_t *binary);

/**
 * @brief Deserialize file header from byte array.
 * @param binary Byte array to deserialize from.
 * @param new_header File header to store result in.
 * @return True if successful, false otherwise.
*/
bool deser_bin_fheader(binary_t *binary, fheader_t *new_header);

/**
 * @brief Deserialize file header from buffer reader.
 * @param reader Buffer reader to deserialize from.
 * @param new_header File header to store result in.
 * @return True if successful, false otherwise.
*/
bool deser_br_fheader(bufreader_t *reader, fheader_t *new_header);

#endif