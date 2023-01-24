cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)    

set(SOURCES 
${PROJECT_SOURCE_DIR}/src/sha3.c
${PROJECT_SOURCE_DIR}/src/meta.c
)

add_executable(c_lusl_test sha_test.c ${SOURCES})
target_include_directories(c_lusl_test PUBLIC ../include)


add_executable(meta_test meta_test.c ${SOURCES})
target_include_directories(meta_test PUBLIC ../include)
