/* crypto/bf/bf_skey.c, bf_enc.c, bf_cfb64.c */
/* Copyright (C) 1995-1997 Eric Young (eay@mincom.oz.au)
 * All rights reserved.
 *
 * See the licence and distribution terms in bf_pi.h, which come from the
 * original MiBench security/blowfish sources and apply to this file too.
 */
/*
 * MiBench security/blowfish.
 *
 * HLS adaptation: 32-bit words, ANSI prototypes, the 16 unrolled rounds of
 * BF_encrypt are a loop, big-endian word access is spelled out instead of
 * the n2l/l2n pointer macros, and the CFB64 stream is processed in place
 * over a fixed-size buffer.
 */
#include "blowfish.h"
#include "bf_pi.h"

/* F function of one round: (((S0[a] + S1[b]) ^ S2[c]) + S3[d]) */
#define BF_F(S, x) \
    (((((S)[(x) >> 24] + (S)[0x0100 + (((x) >> 16) & 0xff)]) ^ (S)[0x0200 + (((x) >> 8) & 0xff)]) + \
      (S)[0x0300 + ((x) & 0xff)]))

void BF_encrypt(uint32_t data[2], const BF_KEY *key)
{
    uint32_t l = data[0];
    uint32_t r = data[1];
    const uint32_t *p = key->P;
    const uint32_t *s = key->S;
    uint32_t t;

    l ^= p[0];
    for (int i = 1; i <= BF_ROUNDS; i += 2) {
        r ^= BF_F(s, l) ^ p[i];
        l ^= BF_F(s, r) ^ p[i + 1];
    }
    r ^= p[BF_ROUNDS + 1];

    t = r;
    r = l;
    l = t;
    data[0] = l;
    data[1] = r;
}

void BF_set_key(BF_KEY *key, int len, const uint8_t *data)
{
    int i, j;
    uint32_t ri;
    uint32_t in[2];
    int d = 0;

    for (i = 0; i < BF_ROUNDS + 2; i++)
        key->P[i] = bf_init_P[i];
    for (i = 0; i < 4 * 256; i++)
        key->S[i] = bf_init_S[i];

    if (len > ((BF_ROUNDS + 2) * 4))
        len = (BF_ROUNDS + 2) * 4;

    for (i = 0; i < BF_ROUNDS + 2; i++) {
        ri = 0;
        for (j = 0; j < 4; j++) {
            ri = (ri << 8) | data[d++];
            if (d >= len)
                d = 0;
        }
        key->P[i] ^= ri;
    }

    in[0] = 0;
    in[1] = 0;
    for (i = 0; i < BF_ROUNDS + 2; i += 2) {
        BF_encrypt(in, key);
        key->P[i] = in[0];
        key->P[i + 1] = in[1];
    }
    for (i = 0; i < 4 * 256; i += 2) {
        BF_encrypt(in, key);
        key->S[i] = in[0];
        key->S[i + 1] = in[1];
    }
}

void blowfish_cfb64(const uint8_t key[BF_KEY_LEN], const uint8_t ivec[8],
                    const uint8_t in[BF_DATA_LEN], uint8_t out[BF_DATA_LEN],
                    int encrypt)
{
    BF_KEY schedule;
    uint8_t iv[8];
    uint32_t ti[2];
    int n = 0;

    BF_set_key(&schedule, BF_KEY_LEN, key);
    for (int i = 0; i < 8; i++)
        iv[i] = ivec[i];

    for (int l = 0; l < BF_DATA_LEN; l++) {
        if (n == 0) {
            ti[0] = ((uint32_t)iv[0] << 24) | ((uint32_t)iv[1] << 16) |
                    ((uint32_t)iv[2] << 8) | (uint32_t)iv[3];
            ti[1] = ((uint32_t)iv[4] << 24) | ((uint32_t)iv[5] << 16) |
                    ((uint32_t)iv[6] << 8) | (uint32_t)iv[7];
            BF_encrypt(ti, &schedule);
            for (int k = 0; k < 4; k++) {
                iv[k] = (uint8_t)(ti[0] >> (24 - 8 * k));
                iv[4 + k] = (uint8_t)(ti[1] >> (24 - 8 * k));
            }
        }
        if (encrypt) {
            uint8_t c = in[l] ^ iv[n];
            out[l] = c;
            iv[n] = c;
        } else {
            uint8_t cc = in[l];
            uint8_t c = iv[n];
            iv[n] = cc;
            out[l] = c ^ cc;
        }
        n = (n + 1) & 0x07;
    }
}
