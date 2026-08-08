#include "mix_cols.h"

void phex(uint8_t *str) {

    uint8_t len = 16;
    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}

int main() {
    state_t state = {{0x00, 0x11, 0x22, 0x33},
                     {0x44, 0x55, 0x66, 0x77},
                     {0x88, 0x99, 0xaa, 0xbb},
                     {0xcc, 0xdd, 0xee, 0xff}};

    MixColumns(&state);
    phex((uint8_t *)state);

    return 0;
}
