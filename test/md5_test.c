#include "md5.h"
#include <stdio.h>
#include <stdint.h>

int main() {
    MD5Context ctx;
    md5Init(&ctx);
    md5Update(&ctx, "abc", 3);
    const uint8_t *hash;
    md5Finalize(&ctx);
    for (int i = 0; i < 16; i++) {
        printf("%02x ", ctx.digest[i]);
    }
    printf("\n");
    return 0;
}