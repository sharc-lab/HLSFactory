#include "add_round_key.h"

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void AddRoundKey(uint8_t round, state_t *state, const uint8_t *RoundKey) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
        }
    }
}