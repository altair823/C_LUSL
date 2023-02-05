#include <stdio.h>
#include <assert.h>

#include "test_util.h"
#include "serializer.h"
#include "deserializer.h"

void mkdir_recursive_test() {
    INIT_TEST_SUITE;
    mkdir_recursive("./deserializer/test1/test2/test3/");
    assert(mkdir_recursive("./deserializer/test1/test2/test3/") == true);
    assert(opendir("./deserializer/test1/test2") != NULL);
    assert(opendir("./deserializer/test1/test2/test3") == NULL);
    remove_directory("./deserializer");
    END_TEST_SUITE;
}

void deserialize_test() {
    INIT_TEST_SUITE;
    char *root_dir = "./deserialize_test";
    char *output_file = "./deserialize_test_output.srl";
    make_directory(root_dir);
    char *subdir1 = "./deserialize_test/dir1";
    char *subdir2 = "./deserialize_test/dir2";
    char *subdir3 = "./deserialize_test/dir2/dir3";
    make_directory(subdir1);
    make_directory(subdir2);
    make_directory(subdir3);
    char *file1 = "./deserialize_test/file1.txt";
    char *file2 = "./deserialize_test/dir1/file2.txt";
    char *file3 = "./deserialize_test/dir2/file3.txt";
    char *file4 = "./deserialize_test/dir2/dir3/file4.txt";
    make_dummy_file(file1, 0, 100);
    make_dummy_file(file2, 0, 200);
    make_dummy_file(file3, 0, 300);
    make_dummy_file(file4, 0, 400);

    INIT_SERIALIZER(serializer, root_dir, output_file);
    SET_OPTION(serializer, 0);
    if (serialize(&serializer)) {
        assert(true);
    } else {
        assert(false && "serialize failed");
    }
    FREE_SERIALIZER(serializer);

    char *output_dir = "./deserialize_test_output";
    INIT_DESERIALIZER(deserializer, output_file, output_dir);
    if (deserialize(&deserializer)) {
        assert(true);
    } else {
        assert(false && "deserialize failed");
    }
    FREE_DESERIALIZER(deserializer);

    remove_directory(root_dir);
    remove_directory(output_dir);
    remove(output_file);

    END_TEST_SUITE;
}

int main() {
    mkdir_recursive_test();
    deserialize_test();
    return 0;
}