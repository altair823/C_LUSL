cmake_minimum_required(VERSION 3.16)

project(c_lusl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)    

set(SOURCES 
${PROJECT_SOURCE_DIR}/src/sha3.c
${PROJECT_SOURCE_DIR}/src/meta.c
${PROJECT_SOURCE_DIR}/src/binary.c
${PROJECT_SOURCE_DIR}/src/header.c
)

# main executable
add_executable(c_lusl main.c ${SOURCES})
target_include_directories(c_lusl PRIVATE include)

# test executable
add_subdirectory (test)


enable_testing()
add_test(NAME sha_test COMMAND sha_test)
add_test(NAME meta_test COMMAND meta_test)
add_test(NAME binary_test COMMAND binary_test)
add_test(NAME header_test COMMAND header_test)