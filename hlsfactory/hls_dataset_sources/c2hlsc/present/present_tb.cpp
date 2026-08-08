#include "present.h"

int main(int argc, char *argv[]) {

    block_t plaintext = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x11};
    present_key_t key = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23};
    present80_encryptBlock(&plaintext, &key);

    for (int i = 0; i < 8; i++)
        printf("%2x ", plaintext[i]);

    return 0;
}