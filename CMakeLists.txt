cmake_minimum_required(VERSION 3.16)


# For windows cross compilation
# if(WIN32)
# set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
# set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
# set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
# set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
# set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# endif()

project(c_lusl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)    


file(GLOB SOURCES
		"${PROJECT_SOURCE_DIR}/src/*.c"
        )

# main executable
add_executable(c_lusl main.c ${SOURCES})
target_include_directories(c_lusl PRIVATE include)
# target_compile_options(c_lusl PUBLIC -Wall -Werror)

# test executable
add_subdirectory (test)


enable_testing()
add_test(NAME sha_test COMMAND sha_test)
add_test(NAME md5_test COMMAND md5_test)
add_test(NAME meta_test COMMAND meta_test)
add_test(NAME binary_test COMMAND binary_test)
add_test(NAME header_test COMMAND header_test)
add_test(NAME bufread_test COMMAND bufread_test)
add_test(NAME serializer_test COMMAND serializer_test)
add_test(NAME deserializer_test COMMAND deserializer_test)