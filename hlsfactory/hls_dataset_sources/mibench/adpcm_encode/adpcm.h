#ifndef ADPCM_H
#define ADPCM_H

/* Samples per block (rawcaudio uses NSAMPLES = 1000) */
#define ADPCM_NSAMPLES 1000

/* state[0] = valprev (previous output value), state[1] = index into step table */
void adpcm_coder(const short indata[ADPCM_NSAMPLES],
                 signed char outdata[ADPCM_NSAMPLES / 2], int state[2]);
void adpcm_decoder(const signed char indata[ADPCM_NSAMPLES / 2],
                   short outdata[ADPCM_NSAMPLES], int state[2]);

#endif
