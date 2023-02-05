#include <stdio.h>
#include <assert.h>

#include "test_util.h"
#include "deserializer.h"

void mkdir_recursive_test() {
    INIT_TEST_SUITE;
    assert(mkdir_recursive("./deserializer/test1/test2/test3") == true);
    assert(opendir("./deserializer/test1/test2/test3") != NULL);
    remove_directory("./deserializer");
    END_TEST_SUITE;
}

int main() {
    mkdir_recursive_test();
    return 0;
}