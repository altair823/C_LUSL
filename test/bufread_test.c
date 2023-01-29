#include <stdio.h>
#include <assert.h>

#include "test_util.h"
#include "bufread.h"

void chunk_test() {
    INIT_TEST_SUITE;
    FILE *file = fopen("bufread_test.txt", "w");
    assert(file != NULL);
    for (int i = 0; i < 50; i++) {
        fprintf(file, "%d", i);
    }
    fclose(file);

    INIT_CHUNK(chunk1);
    file = fopen("bufread_test.txt", "r");
    assert(file != NULL);
    chunk1.length = 10;
    assert(read_chunk(file, &chunk1));
    assert(chunk1.length == 10);
    for (int i = 0; i < chunk1.length; i++) {
        assert(chunk1.data[i] == i + '0');
    }
    FREE_CHUNK(chunk1);

    INIT_CHUNK(chunk2);
    chunk2.length = 10;
    assert(read_chunk(file, &chunk2));
    assert(chunk2.length == 10);
    assert(chunk2.data[0] == '1');
    assert(chunk2.data[1] == '0');

    assert(chunk2.data[2] == '1');
    assert(chunk2.data[3] == '1');

    assert(chunk2.data[4] == '1');
    assert(chunk2.data[5] == '2');

    assert(chunk2.data[6] == '1');
    assert(chunk2.data[7] == '3');

    assert(chunk2.data[8] == '1');
    assert(chunk2.data[9] == '4');
    FREE_CHUNK(chunk2);
    fclose(file);

    file = fopen("bufread_test.txt", "r");
    INIT_CHUNK(big_chunk);
    big_chunk.length = 1000;
    assert(read_chunk(file, &big_chunk));
    printf("chunk2.length = %d\n", big_chunk.length);
    assert(big_chunk.length == 90);
    FREE_CHUNK(big_chunk);
    fclose(file);

    END_TEST_SUITE;
}

int main() {
    chunk_test();
    return 0;
}