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
    file = fopen("bufread_test.txt", "rb");
    assert(file != NULL);
    chunk1.length = 10;
    assert(read_chunk(file, &chunk1, chunk1.length));
    assert(chunk1.length == 10);
    for (int i = 0; i < chunk1.length; i++) {
        assert(chunk1.data[i] == i + '0');
    }
    FREE_CHUNK(chunk1);

    INIT_CHUNK(chunk2);
    chunk2.length = 10;
    assert(read_chunk(file, &chunk2, chunk2.length));
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

    file = fopen("bufread_test.txt", "rb");
    INIT_CHUNK(big_chunk);
    big_chunk.length = 1000;
    assert(read_chunk(file, &big_chunk, big_chunk.length));
    assert(big_chunk.length == 90);
    FREE_CHUNK(big_chunk);
    fclose(file);

    END_TEST_SUITE;
}

void read_bufreader_test() {
    INIT_TEST_SUITE;
    FILE *file = fopen("read_bufreader_test.txt", "w");
    assert(file != NULL);
    for (int i = 0; i < 100; i++) {
        fprintf(file, "%d", i);
    }
    fclose(file);

    file = fopen("bufread_test.txt", "rb");
    assert(file != NULL);

    // Test read_bufreader when binary can fit in buffer.
    // |0123456789|
    INIT_BUFREADER(bufreader, file, 10);
    INIT_BINARY(binary_fit_in_buf);
    assert(read_bufreader(&bufreader, 10, &binary_fit_in_buf));
    assert(binary_fit_in_buf.length == 10);
    for (int i = 0; i < binary_fit_in_buf.length; i++) {
        assert(binary_fit_in_buf.data[i] == i + '0');
    }
    FREE_BINARY(binary_fit_in_buf);

    // Test read_bufreader when binary is smaller than buffer.
    // |101112----|
    INIT_BINARY(binary_smaller_than_buf);
    assert(read_bufreader(&bufreader, 6, &binary_smaller_than_buf));
    assert(binary_smaller_than_buf.length == 6);
    assert(binary_smaller_than_buf.data[0] == '1');
    assert(binary_smaller_than_buf.data[1] == '0');
    assert(binary_smaller_than_buf.data[2] == '1');
    assert(binary_smaller_than_buf.data[3] == '1');
    assert(binary_smaller_than_buf.data[4] == '1');
    assert(binary_smaller_than_buf.data[5] == '2');
    FREE_BINARY(binary_smaller_than_buf);

    // Test read_bufreader when binary data span the buffer.
    // |------1314|
    // |15161718--|
    INIT_BINARY(binary_over_buf);
    assert(read_bufreader(&bufreader, 12, &binary_over_buf));
    assert(binary_over_buf.length == 12);
    assert(binary_over_buf.data[0] == '1');
    assert(binary_over_buf.data[1] == '3');
    assert(binary_over_buf.data[2] == '1');
    assert(binary_over_buf.data[3] == '4');
    assert(binary_over_buf.data[4] == '1');
    assert(binary_over_buf.data[5] == '5');
    assert(binary_over_buf.data[6] == '1');
    assert(binary_over_buf.data[7] == '6');
    assert(binary_over_buf.data[8] == '1');
    assert(binary_over_buf.data[9] == '7');
    assert(binary_over_buf.data[10] == '1');
    assert(binary_over_buf.data[11] == '8');
    FREE_BINARY(binary_over_buf);

    // Test read_bufreader when binary is bigger than buffer.
    // |--------19|
    // |2021222324|
    // |2526272829|
    // |303-------|
    INIT_BINARY(binary_bigger_than_buf);
    assert(read_bufreader(&bufreader, 25, &binary_bigger_than_buf));
    assert(binary_bigger_than_buf.length == 25);
    assert(binary_bigger_than_buf.data[0] == '1');
    assert(binary_bigger_than_buf.data[1] == '9');
    assert(binary_bigger_than_buf.data[2] == '2');
    assert(binary_bigger_than_buf.data[3] == '0');
    assert(binary_bigger_than_buf.data[4] == '2');
    assert(binary_bigger_than_buf.data[5] == '1');
    assert(binary_bigger_than_buf.data[6] == '2');
    assert(binary_bigger_than_buf.data[7] == '2');
    assert(binary_bigger_than_buf.data[8] == '2');
    assert(binary_bigger_than_buf.data[9] == '3');
    assert(binary_bigger_than_buf.data[10] == '2');
    assert(binary_bigger_than_buf.data[11] == '4');
    assert(binary_bigger_than_buf.data[12] == '2');
    assert(binary_bigger_than_buf.data[13] == '5');
    assert(binary_bigger_than_buf.data[14] == '2');
    assert(binary_bigger_than_buf.data[15] == '6');
    assert(binary_bigger_than_buf.data[16] == '2');
    assert(binary_bigger_than_buf.data[17] == '7');
    assert(binary_bigger_than_buf.data[18] == '2');
    assert(binary_bigger_than_buf.data[19] == '8');
    assert(binary_bigger_than_buf.data[20] == '2');
    assert(binary_bigger_than_buf.data[21] == '9');
    assert(binary_bigger_than_buf.data[22] == '3');
    assert(binary_bigger_than_buf.data[23] == '0');
    assert(binary_bigger_than_buf.data[24] == '3');
    FREE_BINARY(binary_bigger_than_buf);

    // Test read_bufreader when binary is padding the buffer.
    // |---1323334|
    INIT_BINARY(binary_padding_buf);
    assert(read_bufreader(&bufreader, 7, &binary_padding_buf));
    assert(binary_padding_buf.length == 7);
    assert(binary_padding_buf.data[0] == '1');
    assert(binary_padding_buf.data[1] == '3');
    assert(binary_padding_buf.data[2] == '2');
    assert(binary_padding_buf.data[3] == '3');
    assert(binary_padding_buf.data[4] == '3');
    assert(binary_padding_buf.data[5] == '3');
    assert(binary_padding_buf.data[6] == '4');
    FREE_BINARY(binary_padding_buf);

    // Temp binary for next test.
    // |353-------|
    INIT_BINARY(binary_for_temp);
    assert(read_bufreader(&bufreader, 3, &binary_for_temp));
    assert(binary_for_temp.length == 3);
    assert(binary_for_temp.data[0] == '3');
    assert(binary_for_temp.data[1] == '5');
    assert(binary_for_temp.data[2] == '3');
    FREE_BINARY(binary_for_temp);

    // Test read_bufreader when binary is padding the buffer and also bigger than buffer.
    // |---6373839|
    // |4041424344|
    INIT_BINARY(binary_big_padding_buf);
    assert(read_bufreader(&bufreader, 17, &binary_big_padding_buf));
    assert(binary_big_padding_buf.length == 17);
    assert(binary_big_padding_buf.data[0] == '6');
    assert(binary_big_padding_buf.data[1] == '3');
    assert(binary_big_padding_buf.data[2] == '7');
    assert(binary_big_padding_buf.data[3] == '3');
    assert(binary_big_padding_buf.data[4] == '8');
    assert(binary_big_padding_buf.data[5] == '3');
    assert(binary_big_padding_buf.data[6] == '9');
    assert(binary_big_padding_buf.data[7] == '4');
    assert(binary_big_padding_buf.data[8] == '0');
    assert(binary_big_padding_buf.data[9] == '4');
    assert(binary_big_padding_buf.data[10] == '1');
    assert(binary_big_padding_buf.data[11] == '4');
    assert(binary_big_padding_buf.data[12] == '2');
    assert(binary_big_padding_buf.data[13] == '4');
    assert(binary_big_padding_buf.data[14] == '3');
    assert(binary_big_padding_buf.data[15] == '4');
    assert(binary_big_padding_buf.data[16] == '4');
    FREE_BINARY(binary_big_padding_buf);

    fclose(file);
    
    END_TEST_SUITE;
}

int main() {
    chunk_test();
    read_bufreader_test();
    return 0;
}