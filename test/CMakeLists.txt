cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)    

set(SOURCES 
${PROJECT_SOURCE_DIR}/src/sha3.c
${PROJECT_SOURCE_DIR}/src/meta.c
${PROJECT_SOURCE_DIR}/src/binary.c
${PROJECT_SOURCE_DIR}/src/header.c
${PROJECT_SOURCE_DIR}/src/bufread.c
${PROJECT_SOURCE_DIR}/src/serializer.c
)

add_executable(sha_test sha_test.c ${SOURCES})
target_include_directories(sha_test PUBLIC ../include)

add_executable(meta_test meta_test.c ${SOURCES})
target_include_directories(meta_test PUBLIC ../include)

add_executable(binary_test binary_test.c ${SOURCES})
target_include_directories(binary_test PUBLIC ../include)

add_executable(header_test header_test.c ${SOURCES})
target_include_directories(header_test PUBLIC ../include)

add_executable(bufread_test bufread_test.c ${SOURCES})
target_include_directories(bufread_test PUBLIC ../include)

add_executable(serializer_test serializer_test.c ${SOURCES})
target_include_directories(serializer_test PUBLIC ../include)
