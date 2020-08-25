/*
 * tfmx_audio.c
 * Paula emulator.
 * jhp 20Mar96
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include "tfmx_audio.h"
#include "tfmx_player.h"

static void conv_s16(TfmxState* state, S32* buf, int nsamples);
static void mix_add(TfmxState* state, struct Hdb* hw, int n, S32* b);

static void (*convert_func)(TfmxState*, S32*, int) = &conv_s16;
static void (*mixing_func)(TfmxState*, struct Hdb*, int, S32*) = &mix_add;

static unsigned char* tfmx_get_next_buffer(TfmxState* state) {
    unsigned char* buffer = &state->global_buf_union.b8[state->btail];
    int n = state->blocksize * state->bytes_per_sample;

    if (!state->bqueue)
        return NULL;

    state->btail = (state->btail + n) & BOFSIZE;
    state->bqueue--;

    return buffer;
}

/* Simple little three-position weighted-sum LPF. */

static void filter(TfmxState* state, S32* buf, int nsamples) {
    static int wl = 0;
    static int wr = 0; /* actually backwards but who cares? */
    int i;

    switch (state->plugin_cfg.filt) {
        case 3:
            for (i = 0; i < nsamples; i++) {
                wl = ((buf[HALFBUFSIZE]) + wl * 3) / 4;
                buf[HALFBUFSIZE] = wl;

                wr = (*buf + wr * 3) / 4;
                *buf++ = wr;
            }
            break;
        case 2:
            for (i = 0; i < nsamples; i++) {
                wl = ((buf[HALFBUFSIZE]) + wl) / 2;
                buf[HALFBUFSIZE] = wl;

                wr = (*buf + wr) / 2;
                *buf++ = wr;
            }
            break;
        case 1:
            for (i = 0; i < nsamples; i++) {
                wl = ((buf[HALFBUFSIZE]) * 3 + wl) / 4;
                buf[HALFBUFSIZE] = wl;

                wr = (*buf * 3 + wr) / 4;
                *buf++ = wr;
            }
            break;
    }
}

/* This one looks like a good candidate for high optimization... */

static void stereoblend(TfmxState* state, S32* buf, int nsamples) {
    if (state->plugin_cfg.blend) {
        int i;

        for (i = 0; i < nsamples; i++) {
            int y;

            y = ((buf[HALFBUFSIZE] * 11) + (*buf * 5)) / 16;
            *buf = ((buf[HALFBUFSIZE] * 5) + (*buf * 11)) / 16;
            buf[HALFBUFSIZE] = y;
            buf++;
        }
    }
}

static void conv_u8(TfmxState* state, S32* buf, int nsamples) {
    int i;
    S32* c = buf;
    U8* a = (U8*)&state->global_buf_union.b8[state->bhead];
    // register int l;

    state->bhead = (state->bhead + (nsamples * state->bytes_per_sample)) & BOFSIZE;

    filter(state, buf, nsamples);
    stereoblend(state, buf, nsamples);

    if (state->output_chans == 2) {
        for (i = 0; i < nsamples; i++) {
            *a++ = ((buf[HALFBUFSIZE]) / 256) ^ 0x80;
            *a++ = ((*buf++) / 256) ^ 0x80;
        }
    } else {
        for (i = 0; i < nsamples; i++) {
            S32 t = *buf++;
            *a++ = ((buf[HALFBUFSIZE] + t) / 512) ^ 0x80;
        }
    }

    state->bytes2 += nsamples;
    for (i = 0; i < nsamples; i++) {
        c[HALFBUFSIZE] = 0;
        *c++ = 0;
    }
}

static void conv_s16(TfmxState* state, S32* buf, int nsamples) {
    int i;
    S32* c = buf;
    S16* a = (S16*)&state->global_buf_union.b8[state->bhead];
    // register int l;

    state->bhead = (state->bhead + (nsamples * state->bytes_per_sample)) & BOFSIZE;

    filter(state, buf, nsamples);
    stereoblend(state, buf, nsamples);

    if (state->output_chans == 2) {
        for (i = 0; i < nsamples; i++) {
            *a++ = buf[HALFBUFSIZE];
            *a++ = *buf++;
        }
    } else /* mono */
    {
        for (i = 0; i < nsamples; i++) {
            *a++ = (buf[HALFBUFSIZE] + *buf) / 2;
            buf++;
        }
    }

    state->bytes2 += nsamples;

    for (i = 0; i < nsamples; i++) {
        c[HALFBUFSIZE] = 0;
        *c++ = 0;
    }
}

static void mix_add(TfmxState* state, struct Hdb* hw, int n, S32* b) {
    register S8* smpl = hw->sbeg;
    register U32 pos = hw->pos;
    int vol = hw->vol;
    U32 delta = hw->delta;
    U32 l = hw->slen << 14;

    if (hw->SampleStart < (S8*)state->smplbuf || hw->sbeg < (S8*)state->smplbuf)
        return;
    if (hw->SampleStart >= (S8*)state->smplbuf_end || hw->sbeg >= (S8*)state->smplbuf_end)
        return;

    if (vol > 0x40)
        vol = 0x40;

    /* This used to have (p==&smplbuf).  Broke with GrandMonsterSlam */
    if ((smpl == (S8*)&state->nul) || ((hw->mode & 1) == 0) || (l < 0x10000L))
        return;

    if ((hw->mode & 3) == 1) {
        smpl = hw->sbeg = hw->SampleStart;
        hw->slen = hw->SampleLength;
        l = hw->slen << 14;
        pos = 0;
        hw->mode |= 2;
        /*		hw->loop(&hw);*/
    }

    while (n--) {
        /* p[] went out of smplbuf, when SetBegin messed up c->SaveAddr (sbeg): */
        /* p[] goes negative on repeated AddBegin:s (R-Type) */
        S32 tmpReg;

        pos += delta;
        tmpReg = smpl[pos / 16384] * vol;
        *b++ += tmpReg;

        if (pos < l)
            continue;
        pos -= l;
        smpl = hw->SampleStart;

        hw->slen = hw->SampleLength;
        l = hw->slen << 14;

        if ((l < 0x10000) || (!hw->loop(hw))) {
            hw->slen = 0;
            pos = 0;
            delta = 0;
            smpl = (S8*)state->smplbuf;
            break;
        }
    }

    hw->sbeg = smpl;
    hw->pos = pos;
    hw->delta = delta;

    if (hw->mode & 4)
        hw->mode = 0;
}

static void mix_add_ov(TfmxState* state, struct Hdb* hw, int n, S32* b) {
    register S8* smpl = hw->sbeg;
    register U32 pos = hw->pos;
    register U32 psreal;
    int v = hw->vol;
    U32 delta = hw->delta;
    U32 l = hw->slen << 14;

    int v1;
    int v2;

    if (v > 0x40)
        v = 0x40;

    /* This used to have (p==&smplbuf).  Broke with GrandMonsterSlam */
    if ((smpl == (S8*)&state->nul) || ((hw->mode & 1) == 0) || (l < 0x10000))
        return;

    if ((hw->mode & 3) == 1) {
        smpl = hw->sbeg = hw->SampleStart;
        hw->slen = hw->SampleLength;
        l = hw->slen << 14;
        pos = 0;
        hw->mode |= 2;
        /*  	hw->loop(&hw); */
    }

/*
  #   define RESAMPLATION \
  v1=src[ofs>>FRACTION_BITS];\
  v2=src[(ofs>>FRACTION_BITS)+1];\
  *dest++ = v1 + (((v2-v1) * (ofs & FRACTION_MASK)) >> FRACTION_BITS);

  */
#define FRACTION_BITS 14
#define INTEGER_MASK (0xFFFFFFFF << FRACTION_BITS)
#define FRACTION_MASK (~INTEGER_MASK)

    while (n--) {
        /*  	register short oo = (short)(pos & 0x3FFF); */
        /*  	q = smpl[pos / 16384 + 1] * (16384 - oo); */
        /*  	pos += delta; */
        /*  	*b++ += (smpl[pos / 16384] * oo + q) * v / 16384; */

        /*
          (*b++)+=(p[ps>>14]*v);
        */
        psreal = pos >> FRACTION_BITS;
        v1 = smpl[psreal];
        if (psreal + 1 < hw->slen) {
            v2 = smpl[psreal + 1];
        } else {
            v2 = hw->SampleStart[0];
            /* fprintf(stderr, "H"); */
            /* (*b++) += v*v1; */
        }
        (*b++) += v * ((v1 + (((signed)((v2 - v1) * (pos & FRACTION_MASK))) >> FRACTION_BITS)));
        pos += delta;

        if (pos < l)
            continue;
        pos -= l;
        smpl = hw->SampleStart;

        hw->slen = hw->SampleLength;
        l = hw->slen << 14;

        if ((l < 0x10000) || (!hw->loop(hw))) {
            hw->slen = 0;
            pos = 0;
            delta = 0;
            smpl = (S8*)state->smplbuf;
            break;
        }
    }

    hw->sbeg = smpl;
    hw->pos = pos;
    hw->delta = delta;

    if (hw->mode & 4)
        (hw->mode = 0);
}

static inline S32 clamp(S32 v, S32 low, S32 high) {
    if (v < low)
        return low;

    if (v > high)
        return high;

    return v;
}

/*
 * mix all used channels, depending of the user enabled voices
 */
static void mixit(TfmxState* state, int nb, int bd) {
    int i;
    S32* ptr;

    if (state->multimode) {
        if (state->active_voice[4])
            mixing_func(state, &state->hdb[4], nb, &state->tbuf[bd]);
        if (state->active_voice[5])
            mixing_func(state, &state->hdb[5], nb, &state->tbuf[bd]);
        if (state->active_voice[6])
            mixing_func(state, &state->hdb[6], nb, &state->tbuf[bd]);
        if (state->active_voice[7])
            mixing_func(state, &state->hdb[7], nb, &state->tbuf[bd]);

        ptr = &state->tbuf[HALFBUFSIZE + bd];
        for (i = 0; i < nb; i++) {
            *ptr = clamp(*ptr, -16383, 16383);
            ptr++;
        }
    } else if (state->active_voice[3])
        mixing_func(state, &state->hdb[3], nb, &state->tbuf[bd]);

    if (state->active_voice[0])
        mixing_func(state, &state->hdb[0], nb, &state->tbuf[bd]);
    if (state->active_voice[1])
        mixing_func(state, &state->hdb[1], nb, &state->tbuf[HALFBUFSIZE + bd]);
    if (state->active_voice[2])
        mixing_func(state, &state->hdb[2], nb, &state->tbuf[HALFBUFSIZE + bd]);
}

static void mixem(TfmxState* state, U32 nb, U32 bd) {
    //	int x;
    /*	printf("nb=%5d bd=%5d\n",nb,bd);*/
    if (state->plugin_cfg.over)
        mixing_func = &mix_add_ov;
    else
        mixing_func = &mix_add;
    mixit(state, nb, bd);
    /*	printf("%6d at byte %4x (made %4x bytes) %3xbpm\n",
                   jiffies,bd,nb,0x1B51F8/eClocks);*/
    /*	if (mix==&mix_set)*/
}

long tfmx_get_block_size(TfmxState* state) {
	(void)state;
    return HALFBUFSIZE;
} /* *4; } */
/*{ return blocksize*bytes_per_sample; }*/

int tfmx_get_block(TfmxState* state, void* buffer) {
    unsigned char* newBuf;

    newBuf = tfmx_get_next_buffer(state);
    if (!newBuf)
        return 0;

    /* Write a new buffered block */
    memcpy((char*)buffer, newBuf, tfmx_get_block_size(state));

    return 1;
}

void tfmx_calc_sizes(TfmxState* state) {
    state->blocksize = HALFBUFSIZE;

    if (!state->force8) {
        convert_func = &conv_s16;
        state->bytes_per_sample = 2;
        printf("using 16 bit\n");
    } else {
        convert_func = &conv_u8;
        state->bytes_per_sample = 1;
    }
    state->bytes_per_sample *= state->output_chans;
    state->blocksize /= state->bytes_per_sample;

    if (state->blocksize > HALFBUFSIZE) {
        // TFMXERR("tfmx_calc_sizes: Block size %d not supported!");
        exit(1);
    }
}

void TfmxTakedown(TfmxState* state) {
    if (state->smplbuf) {
        free(state->smplbuf);
        state->smplbuf = 0;
    }
}

void TfmxResetBuffers(TfmxState* state) {
    state->bhead = 0;
    state->btail = 0;
    state->bqueue = 0;
    state->bytes = 0;
    state->bytes2 = 0;
}

int tfmx_try_to_make_block(TfmxState* state) {
    static S32 nb = 0; /* num bytes */
    static S32 bd = 0; /* bytes done */
    int n;
    int r = 0;  //,m;

    while ((r == 0) && ((state->bqueue + 2) < (BUFSIZE / (state->blocksize * state->bytes_per_sample)))) {
        player_tfmxIrqIn(state);
        nb = (state->eClocks * (state->outRate / 2));
        state->eRem += (nb % 357955);
        nb /= 357955;
        if (state->eRem > 357955) {
            nb++;
            state->eRem -= 357955;
        }
        while (nb > 0) {
            n = state->blocksize - bd;
            if (n > nb)
                n = nb;
            mixem(state, n, bd);
            state->bytes += n;
            bd += n;
            nb -= n;
            if (bd == state->blocksize) {
                convert_func(state, state->tbuf, bd);
                bd = 0;
                state->bqueue++;
                // printf("make %d\n",bqueue);
                r++;
            }
        }
    }
    return ((state->mdb.PlayerEnable) ? r : -1);
}
