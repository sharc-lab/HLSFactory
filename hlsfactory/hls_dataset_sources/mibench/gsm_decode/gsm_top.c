/*
 * MiBench telecomm/gsm: block wrapper around the GSM 06.10 library of
 * Jutta Degener and Carsten Bormann (Technische Universitaet Berlin; see
 * COPYRIGHT).
 *
 * HLS adaptation: the malloc'ed codec handle of gsm_create()/gsm_destroy()
 * is a local state structure, reset the way gsm_create() does (zeroed,
 * nrp = 40), and the untoast driver (file and audio I/O) is replaced by a
 * loop over fixed-size frame buffers.
 */
#include "gsm.h"
#include "private.h"
#include "gsm_top.h"

int gsm_decode_block(const unsigned char in[GSM_FRAMES * GSM_FRAME_BYTES],
                     short pcm[GSM_FRAMES * GSM_FRAME_SAMPLES])
{
    struct gsm_state state;
    char *raw = (char *)&state;
    gsm_signal samples[GSM_FRAME_SAMPLES];
    gsm_byte frame[GSM_FRAME_BYTES];
    int status = 0;

    for (unsigned i = 0; i < sizeof(state); i++)
        raw[i] = 0;
    state.nrp = 40;

    for (int f = 0; f < GSM_FRAMES; f++) {
        for (int i = 0; i < GSM_FRAME_BYTES; i++)
            frame[i] = in[f * GSM_FRAME_BYTES + i];
        if (gsm_decode(&state, frame, samples) != 0)
            status = -1;
        for (int i = 0; i < GSM_FRAME_SAMPLES; i++)
            pcm[f * GSM_FRAME_SAMPLES + i] = samples[i];
    }
    return status;
}
