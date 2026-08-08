#include "sub.h"

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void SubBytes(state_t *state) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[j][i] = getSBoxValue((*state)[j][i]);
        }
    }
}