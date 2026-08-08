#include "aes.h"

// prints string as hex
void phex(uint8_t *str) {

    uint8_t len = 16;
    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}

int main() {
    struct AES_ctx ctx;
    uint8_t key[16] = {0x11,
                       0x22,
                       0x33,
                       0x44,
                       0x55,
                       0x66,
                       0x77,
                       0x88,
                       0x99,
                       0xaa,
                       0xbb,
                       0xcc,
                       0xdd,
                       0xee,
                       0xff,
                       0x00};
    AES_init_ctx(&ctx, key);
    state_t state = {{0xaa, 0xbb, 0xcc, 0xdd},
                     {0xab, 0xa1, 0x1a, 0xba},
                     {0xb0, 0xc1, 0xd2, 0xe4},
                     {0xbd, 0xaf, 0xfa, 0xff}};
    Cipher(&state, &ctx.RoundKey);

    phex((uint8_t *)state);
    return 0;
}
