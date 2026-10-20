#ifndef GSM_TOP_H
#define GSM_TOP_H

#define GSM_FRAMES 16        /* 20 ms frames processed per call */
#define GSM_FRAME_SAMPLES 160
#define GSM_FRAME_BYTES 33

/*
 * GSM 06.10 full-rate RPE-LTP decoder (untoast) over GSM_FRAMES frames,
 * starting from the reset codec state.  Returns 0, or -1 if a frame
 * does not start with the GSM magic number.
 */
int gsm_decode_block(const unsigned char in[GSM_FRAMES * GSM_FRAME_BYTES],
                     short pcm[GSM_FRAMES * GSM_FRAME_SAMPLES]);

#endif
