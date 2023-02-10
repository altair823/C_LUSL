#ifndef CONFIG_H
#define CONFIG_H

#define MAJOR_VERSION 2
#define MINOR_VERSION 1
#define PATCH_VERSION 0

#define DEBUG 0 // Set to 1 to enable debug asserts.
#if DEBUG == 1
#define DEBUG_MSG(msg) assert(false && msg)
#else
#define DEBUG_MSG(msg) printf(msg); printf("\n")
#endif
#define VERBOSE 1

// #define _WIN32

#endif // CONFIG_H