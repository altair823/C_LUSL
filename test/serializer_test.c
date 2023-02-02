#include <assert.h>
#include <dirent.h>

#include "test_util.h"

#include "serializer.h"

void stack_test() {
    INIT_TEST_SUITE;
    INIT_STACK(stack);
    assert(stack.size == DEFAULT_STACK_SIZE);
    char *str1 = "Hello";
    PUSH_STACK(stack, str1);
    assert(strcmp((char *)stack.data[0], "Hello") == 0);
    char *str2 = "World";
    PUSH_STACK(stack, str2);
    assert(strcmp((char *)stack.data[1], "World") == 0);

    char *p_str;
    POP_STACK(stack, p_str);
    assert(strcmp(p_str, "World") == 0);
    POP_STACK(stack, p_str);
    assert(strcmp(p_str, "Hello") == 0);
    END_TEST_SUITE;
}

void get_file_list_test() {
    INIT_TEST_SUITE;
    char *root_dir = "./get_file_list_test";
    make_directory(root_dir);
    char *subdir1 = "./get_file_list_test/dir1";
    char *subdir2 = "./get_file_list_test/dir2";
    char *subdir3 = "./get_file_list_test/dir2/dir3";
    make_directory(subdir1);
    make_directory(subdir2);
    make_directory(subdir3);
    char *file1 = "./get_file_list_test/file1.txt";
    char *file2 = "./get_file_list_test/dir1/file2.txt";
    char *file3 = "./get_file_list_test/dir2/file3.txt";
    char *file4 = "./get_file_list_test/dir2/dir3/file4.txt";
    make_dummy_file(file1, 0, 100);
    make_dummy_file(file2, 0, 200);
    make_dummy_file(file3, 0, 300);
    make_dummy_file(file4, 0, 400);

    INIT_STACK(file_list);
    get_file_list(root_dir, &file_list);
    assert(file_list.top == 4);
    int count = 0;
    for (int i = 0; i < file_list.top; i++) {
        if (strcmp((char *)file_list.data[i], file1) == 0) {
            count++;
            assert(true);
        } else if (strcmp((char *)file_list.data[i], file2) == 0) {
            count++;
            assert(true);
        } else if (strcmp((char *)file_list.data[i], file3) == 0) {
            count++;
            assert(true);
        } else if (strcmp((char *)file_list.data[i], file4) == 0) {
            count++;
            assert(true);
        } else {
            assert(false);
        }
    }
    assert(count == 4);
    remove_directory(root_dir);
    FREE_STACK(file_list);
    END_TEST_SUITE;
}

void init_serializer_test() {
    INIT_TEST_SUITE;
    char *root_dir = "./init_serializer_test";
    char *output_file = "./init_serializer_test/output";
    INIT_SERIALIZER(serializer, root_dir, output_file);
    SET_OPTION(serializer, COMPRESSED_FLAG | ENCRYPTED_FLAG);
    assert(strcmp(serializer.root_dir, "./init_serializer_test") == 0);
    assert(strcmp(serializer.output_file, "./init_serializer_test/output") == 0);
    assert(serializer.option == (COMPRESSED_FLAG | ENCRYPTED_FLAG));
    assert(serializer.file_list != NULL);
    assert(serializer.file_list->top == 0);
    assert(serializer.file_list->size == DEFAULT_STACK_SIZE);
    assert(serializer.file_list->data != NULL);

    FREE_SERIALIZER(serializer);
    assert(serializer.root_dir == NULL);
    assert(serializer.output_file == NULL);
    assert(serializer.option == 0);
    assert(serializer.file_list == NULL);
    END_TEST_SUITE;
}

void serialize_test() {
    INIT_TEST_SUITE;
    char *root_dir = "./serialize_test";
    char *output_file = "./serialize_test_output.srl";
    make_directory(root_dir);
    char *subdir1 = "./serialize_test/dir1";
    char *subdir2 = "./serialize_test/dir2";
    char *subdir3 = "./serialize_test/dir2/dir3";
    make_directory(subdir1);
    make_directory(subdir2);
    make_directory(subdir3);
    char *file1 = "./serialize_test/file1.txt";
    char *file2 = "./serialize_test/dir1/file2.txt";
    char *file3 = "./serialize_test/dir2/file3.txt";
    char *file4 = "./serialize_test/dir2/dir3/file4.txt";
    make_dummy_file(file1, 0, 100);
    make_dummy_file(file2, 0, 200);
    make_dummy_file(file3, 0, 300);
    make_dummy_file(file4, 0, 400);

    INIT_SERIALIZER(serializer, root_dir, output_file);
    SET_OPTION(serializer, 0);
    serialize(&serializer);
    FREE_SERIALIZER(serializer);
    // remove(output_file);
    // remove_directory(root_dir);
    END_TEST_SUITE;
}

int main() {
    stack_test();
    get_file_list_test();
    init_serializer_test();
    serialize_test();
    return 0;
}