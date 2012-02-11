/* 
 * tfmx_audio.c
 * Paula emulator.
 * jhp 20Mar96
 */

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>
#include "tfmx_player.h"
#include "tfmx_audio.h"
#include "xmms_tfmx.h"

char active_voice[8]={1,1,1,1,1,1,1,1};

// The time in milliseconds per buffer that the attempt to write another
// buffer is delayed...
#define BUFDELAY 50

int bhead = 0;
int btail = 0;
int bqueue = 0;
S32 tbuf[HALFBUFSIZE*2];
int bytes = 0;
int bytes2 = 0;
U32 blocksize = 0;
U32 output_chans = 2;
#ifndef NO_SCHED
int sched = 1;
#endif
int isfile = 0;
int eRem = 0;		/* remainder of eclocks */

static int force8 = 0;
static U32 bytes_per_sample = 1; /* range : 1 to 4 (16 bits stereo) */
static int nul = 0;

static union {
 S16 b16[BUFSIZE/2];
 U8 b8[BUFSIZE];
} global_buf_union;


static void conv_s16 (S32 *buf, int nsamples);
static void mix_add (struct Hdb *hw, int n, S32 *b);

static void (*convert_func)(S32 *, int) = &conv_s16;
static void (*mixing_func)(struct Hdb *, int, S32 *) = &mix_add;


static unsigned char *
tfmx_get_next_buffer()
{
    unsigned char *buffer = &global_buf_union.b8[btail];
    int n = blocksize * bytes_per_sample;

    if (!bqueue)
	return NULL;

    btail = (btail + n) & BOFSIZE;
    bqueue--;

    return buffer;
}


/* Simple little three-position weighted-sum LPF. */

static void 
filter(S32 *buf, int nsamples)
{
    static int wl = 0;
    static int wr = 0; /* actually backwards but who cares? */
    int i;

    switch(plugin_cfg.filt)
    {
	case 3:
	    for (i = 0; i < nsamples; i++)
	    {
		wl = ((buf[HALFBUFSIZE]) + wl * 3) / 4;
		buf[HALFBUFSIZE] = wl;

		wr = (*buf + wr * 3) / 4;
		*buf++ = wr;
	    }
	    break;
	case 2:
	    for (i = 0; i < nsamples; i++)
	    {
		wl = ((buf[HALFBUFSIZE]) + wl) / 2;
		buf[HALFBUFSIZE] = wl;

		wr = (*buf + wr) / 2;
		*buf++ = wr;
	    }
	    break;
	case 1:
	    for (i = 0; i < nsamples; i++)
	    {
		wl = ((buf[HALFBUFSIZE]) * 3 + wl) / 4;
		buf[HALFBUFSIZE] = wl;

		wr = (*buf * 3 + wr) / 4;
		*buf++ = wr;
	    }
	    break;
    }
}

/* This one looks like a good candidate for high optimization... */

static void 
stereoblend(S32 *buf, int nsamples)
{
    if (plugin_cfg.blend)
    {
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

static void 
conv_u8 (S32 *buf, int nsamples)
{
    int i;
    S32 *c = buf;
    U8 *a = (U8 *)&global_buf_union.b8[bhead];
    //register int l;

    bhead = (bhead + (nsamples * bytes_per_sample)) & BOFSIZE;

    filter (buf, nsamples);
    stereoblend (buf,nsamples);
	
    if (output_chans == 2)
    {
	for (i = 0; i < nsamples; i++)
	{
	    *a++ = ((buf[HALFBUFSIZE]) / 256) ^ 0x80;
	    *a++ = ((*buf++) / 256) ^ 0x80;
	}
    }
    else
    {
	for (i = 0; i < nsamples; i++) 
	    *a++ = ((buf[HALFBUFSIZE] + *buf++) / 512) ^ 0x80;
    }

    bytes2 += nsamples;
    for(i = 0; i < nsamples ; i++)
    {
	c[HALFBUFSIZE] = 0;
	*c++ = 0;
    }
}

static void 
conv_s16(S32 *buf, int nsamples)
{
    int i;
    S32 *c = buf;
    S16 *a = (S16 *)&global_buf_union.b8[bhead];
    //register int l;
	
    bhead = (bhead + (nsamples * bytes_per_sample)) & BOFSIZE;

    filter (buf, nsamples);
    stereoblend (buf, nsamples);

    if (output_chans == 2)
    {
	for (i = 0; i < nsamples; i++)
	{
	    *a++ = buf[HALFBUFSIZE];
	    *a++ = *buf++;
	}
    }
    else /* mono */
    {
	for (i = 0; i < nsamples; i++)
	{
	    *a++ = (buf[HALFBUFSIZE] + *buf) / 2;
	    buf++;
	}
    }

    bytes2 += nsamples;

    for(i = 0; i < nsamples; i++)
    {
	c[HALFBUFSIZE] = 0;
	*c++ = 0;
    }
}

static void 
mix_add(struct Hdb *hw, int n, S32 *b)
{
    register S8 *smpl = hw->sbeg;
    register U32 pos = hw->pos;
    int vol = hw->vol;
    U32 delta = hw->delta;
    U32 l = hw->slen << 14;

    if (hw->SampleStart < smplbuf || hw->sbeg < smplbuf)
	return;
    if (hw->SampleStart >= smplbuf_end || hw->sbeg >= smplbuf_end)
	return;

    if (vol > 0x40)
	vol = 0x40;

/* This used to have (p==&smplbuf).  Broke with GrandMonsterSlam */
    if ((smpl == (S8 *)&nul) || ((hw->mode & 1) == 0) || (l < 0x10000L))
	return;

    if ((hw->mode & 3) == 1)
    {
	smpl = hw->sbeg = hw->SampleStart;
	hw->slen = hw->SampleLength;
	l = hw->slen << 14;
	pos = 0;
	hw->mode |= 2;
/*		hw->loop(&hw);*/
    }

    while(n--)
    {
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

	if ( (l < 0x10000) || (!hw->loop(hw)) )
	{
	    hw->slen = 0;
	    pos = 0;
	    delta = 0;
	    smpl = smplbuf;
	    break;
	}
    }
	
    hw->sbeg = smpl;
    hw->pos = pos;
    hw->delta = delta;

    if (hw->mode & 4)
	hw->mode = 0;
}
	

static void 
mix_add_ov(struct Hdb *hw, int n, S32 *b)
{
    register S8 *smpl = hw->sbeg;
    register U32 pos = hw->pos;
    register U32 psreal;
    int v = hw->vol;
    U32 delta = hw->delta;
    U32 l = hw->slen << 14;

    int v1;
    int v2;

    int q;

    if (v > 0x40)
	v = 0x40;

/* This used to have (p==&smplbuf).  Broke with GrandMonsterSlam */
    if ((smpl == (S8 *)&nul) || ((hw->mode & 1) == 0) || (l < 0x10000))
	return;

    if ((hw->mode & 3) == 1)
    {
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
#define FRACTION_MASK (~ INTEGER_MASK)

    while(n--)
    {
/*  	register short oo = (short)(pos & 0x3FFF); */
/*  	q = smpl[pos / 16384 + 1] * (16384 - oo); */
/*  	pos += delta; */
/*  	*b++ += (smpl[pos / 16384] * oo + q) * v / 16384; */

	/*
	  (*b++)+=(p[ps>>14]*v);
	*/
	psreal = pos>>FRACTION_BITS;
	v1 = smpl[psreal];
	if (psreal+1 < hw->slen) {	
	    v2 = smpl[psreal+1];
	} else {
	    v2 = hw->SampleStart[0];
	    /* fprintf(stderr, "H"); */
	    /* (*b++) += v*v1; */
	}
	(*b++) += v*((v1 + 
		      (((signed) ((v2-v1) * (pos & FRACTION_MASK)))
		       >> FRACTION_BITS)));
	pos += delta;

	if (pos < l)
	    continue;
	pos -= l;
	smpl = hw->SampleStart;
	
	hw->slen = hw->SampleLength;
	l = hw->slen << 14;

	if ( (l < 0x10000) || (!hw->loop(hw)) )
	{
	    hw->slen = 0;
	    pos = 0;
	    delta = 0;
	    smpl = smplbuf;
	    break;
	}
    }

    hw->sbeg = smpl;
    hw->pos = pos;
    hw->delta = delta;

    if (hw->mode & 4)
	(hw->mode = 0);
}
	

/*
 * mix all used channels, depending of the user enabled voices
 */
static void mixit(int nb, int bd)
{
    int i;
    S32 *ptr;

    if (multimode)
    { 
	if(active_voice[4])
	    mixing_func(&hdb[4], nb, &tbuf[bd]);
	if(active_voice[5])
	    mixing_func(&hdb[5], nb, &tbuf[bd]);
	if(active_voice[6])
	    mixing_func(&hdb[6], nb, &tbuf[bd]);
	if(active_voice[7])
	    mixing_func(&hdb[7], nb, &tbuf[bd]);

	ptr = &tbuf[HALFBUFSIZE + bd];
	for (i = 0; i < nb; i++)
	{
	    *ptr = CLAMP(*ptr, -16383, 16383);
	    ptr++;
	}
    }
    else if (active_voice[3])
	mixing_func(&hdb[3], nb, &tbuf[bd]);

    if(active_voice[0])
	mixing_func(&hdb[0], nb, &tbuf[bd]);
    if(active_voice[1])
	mixing_func(&hdb[1], nb, &tbuf[HALFBUFSIZE + bd]);
    if(active_voice[2])
	mixing_func(&hdb[2], nb, &tbuf[HALFBUFSIZE + bd]);
}

static void mixem(U32 nb, U32 bd)
{
//	int x;
/*	printf("nb=%5d bd=%5d\n",nb,bd);*/
	if (plugin_cfg.over)
	    mixing_func = &mix_add_ov;
	else
	    mixing_func = &mix_add;
	mixit(nb, bd);
/*	printf("%6d at byte %4x (made %4x bytes) %3xbpm\n",
	       jiffies,bd,nb,0x1B51F8/eClocks);*/
/*	if (mix==&mix_set)*/
}


long 
tfmx_get_block_size(void)
{
    return HALFBUFSIZE;
} /* *4; } */
/*{ return blocksize*bytes_per_sample; }*/

int 
tfmx_get_block(void *buffer)
{
    unsigned char *newBuf;

    newBuf = tfmx_get_next_buffer();
    if(!newBuf)
	return 0;

    /* Write a new buffered block */
    memcpy((char *)buffer, newBuf, tfmx_get_block_size());

    return 1;
}

void
tfmx_calc_sizes(void)
{
    blocksize = HALFBUFSIZE;

    if (!force8)
    {
	convert_func = &conv_s16;
	bytes_per_sample = 2;
    }
    else
    {
	convert_func = &conv_u8;
	bytes_per_sample = 1;
    }
    bytes_per_sample *= output_chans;
    blocksize /= bytes_per_sample;

    if (blocksize > HALFBUFSIZE)
    {
	//TFMXERR("tfmx_calc_sizes: Block size %d not supported!");
	exit(1);
    }
}

void 
TfmxTakedown(void)
{
	if(smplbuf)
	{
	    free(smplbuf);
	    smplbuf = 0;
	}
}

void 
TfmxResetBuffers(void)
{
	bhead = 0;
	btail = 0;
	bqueue = 0;
	bytes = 0;
	bytes2 = 0;
}

int 
tfmx_try_to_make_block(void)
{
    static S32 nb = 0; /* num bytes */
    static S32 bd = 0; /* bytes done */
    int n;
    int r = 0; //,m;

    while ((r == 0) &&
	   ((bqueue + 2) < (BUFSIZE / (blocksize * bytes_per_sample))))
    {
	player_tfmxIrqIn();
	nb = (eClocks * (outRate / 2));
	eRem += (nb % 357955);
	nb /= 357955;
	if (eRem > 357955)
	{
	    nb++;
	    eRem -= 357955;
	}
	while (nb > 0)
	{
	    n = blocksize - bd;
	    if (n > nb)
		n = nb;
	    mixem(n, bd);
	    bytes += n;
	    bd += n;
	    nb -= n;
	    if (bd == blocksize)
	    {
		convert_func(tbuf, bd);
		bd = 0;
		bqueue++; 
				//printf("make %d\n",bqueue);
		r++;
	    }
	}
    }
    return((mdb.PlayerEnable) ? r : -1);
}
