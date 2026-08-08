#include "add_round_key.h"

void phex(uint8_t *str) {

    uint8_t len = 16;
    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}
// This function adds the round key to state.
int main() {
    uint8_t rkey[176];
    for (int i = 0; i < 176; i++) {
        rkey[i] = i;
    }

    state_t state = {{0xaa, 0xaa, 0xaa, 0xaa},
                     {0xaa, 0xaa, 0xaa, 0xaa},
                     {0xaa, 0xaa, 0xaa, 0xaa},
                     {0xaa, 0xaa, 0xaa, 0xaa}};

    AddRoundKey(0, &state, rkey);
    phex((uint8_t *)state);
    AddRoundKey(1, &state, rkey);
    phex((uint8_t *)state);
    AddRoundKey(2, &state, rkey);
    phex((uint8_t *)state);
    AddRoundKey(3, &state, rkey);
    phex((uint8_t *)state);
    return 0;
}