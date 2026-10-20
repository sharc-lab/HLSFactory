#ifndef GSM_TOP_H
#define GSM_TOP_H

#define GSM_FRAMES 16        /* 20 ms frames processed per call */
#define GSM_FRAME_SAMPLES 160
#define GSM_FRAME_BYTES 33

/*
 * GSM 06.10 full-rate RPE-LTP encoder (toast) over GSM_FRAMES frames,
 * starting from the reset codec state.
 */
void gsm_encode_block(const short pcm[GSM_FRAMES * GSM_FRAME_SAMPLES],
                      unsigned char out[GSM_FRAMES * GSM_FRAME_BYTES]);

#endif
