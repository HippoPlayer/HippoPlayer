/*
 * tfmx.h
 * Defines and typedefs for the player.
 * jhp 18Mar96
 */

#ifndef __TFMX_H
#define __TFMX_H

#define DEBUGLVL 0
#define DEBUG(x) if (x<DEBUGLVL)

/* arch-dependent stuff here */

typedef unsigned long U32;
typedef unsigned short U16;
typedef unsigned char U8;
typedef long S32;
typedef short S16;
typedef char S8;

/* For your architecture, with 0x12345678 in l, we need to have;
 * w0=0x1234, w1=0x5678, b0=0x12, b1=0x34, b2=0x56, b3=0x78.  This is
 * currently set up for Intel-style little-endian.  VAX and Motorola (among
 * others) will probably have to change this
 */

typedef union {
	U32 l;
	struct {U16 w1,w0;} w;
	struct {U8 b3,b2,b1,b0;} b;
} UNI;

#define MDAT_EDITBUF_LONGS 16384
extern U32 mdat_editbuf[MDAT_EDITBUF_LONGS];

extern U8 *smplbuf;
extern U8 *smplbuf_end;
extern int *patterns;
extern int *macros;
extern int multimode;
extern U32 eClocks;
extern U32 outRate;
extern U32 output_chans;
extern int printinfo;


#define BUFSIZE (8192*2)
#define HALFBUFSIZE (2048*2)

#define BOFSIZE (BUFSIZE-1)


struct TFMXHeader {
    char magic[10];
    char pad[6];
    char text[6][40];
    U16 start[32];
    U16 end[32];
    U16 tempo[32];
    S16 mute[8];
    U32 trackstart;
    U32 pattstart;
    U32 macrostart;
    char pad2[36];
};

extern struct TFMXHeader mdat_header;
extern int num_pat;
extern int num_ts;

#endif
