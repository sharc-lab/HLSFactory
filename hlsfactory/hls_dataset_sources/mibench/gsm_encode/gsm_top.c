/*
 * MiBench telecomm/gsm: block wrapper around the GSM 06.10 library of
 * Jutta Degener and Carsten Bormann (Technische Universitaet Berlin; see
 * COPYRIGHT).
 *
 * HLS adaptation: the malloc'ed codec handle of gsm_create()/gsm_destroy()
 * is a local state structure, reset the way gsm_create() does (zeroed,
 * nrp = 40), and the toast driver (file and audio I/O) is replaced by a
 * loop over fixed-size frame buffers.
 */
#include "gsm.h"
#include "private.h"
#include "gsm_top.h"

void gsm_encode_block(const short pcm[GSM_FRAMES * GSM_FRAME_SAMPLES],
                      unsigned char out[GSM_FRAMES * GSM_FRAME_BYTES])
{
    struct gsm_state state;
    char *raw = (char *)&state;
    gsm_signal samples[GSM_FRAME_SAMPLES];
    gsm_byte frame[GSM_FRAME_BYTES];

    for (unsigned i = 0; i < sizeof(state); i++)
        raw[i] = 0;
    state.nrp = 40;

    for (int f = 0; f < GSM_FRAMES; f++) {
        for (int i = 0; i < GSM_FRAME_SAMPLES; i++)
            samples[i] = pcm[f * GSM_FRAME_SAMPLES + i];
        gsm_encode(&state, samples, frame);
        for (int i = 0; i < GSM_FRAME_BYTES; i++)
            out[f * GSM_FRAME_BYTES + i] = frame[i];
    }
}
