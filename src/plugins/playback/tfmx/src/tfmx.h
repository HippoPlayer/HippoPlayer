/*
 * tfmx.h
 * Defines and typedefs for the player.
 * jhp 18Mar96
 */

#pragma once

#include <stdint.h>

#define DEBUGLVL 0
#define DEBUG(x) if (x < DEBUGLVL)

/* arch-dependent stuff here */

typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;
typedef int S32;
typedef short S16;
typedef char S8;

/* For your architecture, with 0x12345678 in l, we need to have;
 * w0=0x1234, w1=0x5678, b0=0x12, b1=0x34, b2=0x56, b3=0x78.  This is
 * currently set up for Intel-style little-endian.  VAX and Motorola (among
 * others) will probably have to change this
 */

typedef union {
    U32 l;
    struct {
        U16 w1, w0;
    } w;
    struct {
        U8 b3, b2, b1, b0;
    } b;
} UNI;

typedef struct TfmxData {
    U8* data;
    uint64_t size;
    U32 read_offset;
} TfmxData;

#define MDAT_EDITBUF_LONGS 16384
//extern U32 mdat_editbuf[MDAT_EDITBUF_LONGS];

//extern U8* smplbuf;
//extern U8* smplbuf_end;
//extern int* patterns;
//extern int* macros;
//extern int multimode;
//extern U32 eClocks;
//extern U32 outRate;
//extern U32 output_chans;
//extern int printinfo;

#define BUFSIZE (8192 * 2)
#define HALFBUFSIZE (2048 * 2)

#define BOFSIZE (BUFSIZE - 1)

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

typedef struct PluginCfg {
    uint32_t filt;
    uint32_t blend;
    uint32_t over;
} PluginCfg;

struct Hdb {
    U32 pos;
    U32 delta;
    U16 slen;
    U16 SampleLength;
    S8* sbeg;
    S8* SampleStart;
    U8 vol;
    U8 mode;
    int (*loop)(struct Hdb* hw);
    int loopcnt;
    struct Cdb* c;
};

struct Idb {
    U16 Cue[4];
};

struct Mdb {
    char PlayerEnable;
    char EndFlag;
    char CurrSong;
    U16 SpeedCnt;
    U16 CIASave;
    char SongCont;
    char SongNum;
    U16 PlayPattFlag;
    char MasterVol;
    char FadeDest;
    char FadeTime;
    char FadeReset;
    char FadeSlope;
    S16 TrackLoop;
    U16 DMAon;
    U16 DMAoff;
    U16 DMAstate;
    U16 DMAAllow;
};

struct Pdb {
    U32 PAddr;
    U8 PNum;
    S8 PXpose;
    U16 PLoop;
    U16 PStep;
    U8 PWait;
    U16 PRoAddr;
    U16 PRoStep;
};

struct Pdblk {
    U16 FirstPos;
    U16 LastPos;
    U16 CurrPos;
    U16 Prescale;
    struct Pdb p[8];
};

struct Cdb {
    S8 MacroRun, EfxRun;
    U8 NewStyleMacro;
    U8 PrevNote, CurrNote;
    U8 Velocity, Finetune;
    U8 KeyUp, ReallyWait;
    U32 MacroPtr;
    U16 MacroStep, MacroWait, MacroNum;
    S16 Loop;

    U32 CurAddr, SaveAddr;
    U16 CurrLength, SaveLen;

    U16 WaitDMACount;
    U16 DMABit;

    U8 EnvReset, EnvTime, EnvRate;
    S8 EnvEndvol, CurVol;

    S16 VibOffset;
    S8 VibWidth;
    U8 VibFlag, VibReset, VibTime;

    U8 PortaReset, PortaTime;
    U16 CurPeriod, DestPeriod, PortaPer;
    S16 PortaRate;

    U8 AddBeginTime, AddBeginReset;
    U16 ReturnPtr, ReturnStep;
    S32 AddBegin;

    U8 SfxFlag, SfxPriority, SfxNum;
    S16 SfxLockTime;
    U32 SfxCode;

    struct Hdb* hw;
};


// All globals dumped in here to make code thread-safe
typedef struct TfmxState {
    PluginCfg plugin_cfg; // = {2, 1, 1};
    char active_voice[8]; // = {1, 1, 1, 1, 1, 1, 1, 1};
    int bhead; // = 0;
    int btail;//  = 0;
    int bqueue; // = 0;
    S32 tbuf[HALFBUFSIZE * 2];
    int bytes; // = 0;
    int bytes2; // = 0;
    U32 blocksize; // = 0;
    U32 output_chans; // = 2;
#ifndef NO_SCHED
    int sched; // = 1;
#endif
    int isfile; // = 0;
    int eRem; // = 0; // remainder of eclocks

    int force8; // = 0;
    U32 bytes_per_sample; // = 1; range : 1 to 4 (16 bits stereo)
    int nul; // = 0;

    struct Hdb hdb[8];
    struct Mdb mdb;
    struct Cdb cdb[8];
    struct Pdblk pdb;
    /* seems that idb is always written, never read */
    /*  static struct Idb idb; */

    int nSongs;// = 0;
    int jiffies;// = 0;
    int multimode;// = 0;
    U32 eClocks;// = 14318;
    int loops;

    int songnum;

    union {
        S16 b16[BUFSIZE / 2];
        U8 b8[BUFSIZE];
    } global_buf_union;

    char act[8];

    U32 outRate;// = 48000;
    unsigned int mlen; // = 0;
    U8* smplbuf; // = 0; //
    U8* smplbuf_end; // = 0;
    int* macros; // = 0;
    int* patterns; // 0
    short ts[512][8];

    struct TFMXHeader mdat_header;
    U32 mdat_editbuf[MDAT_EDITBUF_LONGS];

    int num_ts;
    int num_pat;
    int num_mac;
    int gubed; // = 0;
    int printinfo;// = 0;

} TfmxState;

void TfmxState_init(TfmxState* state);

//extern struct TFMXHeader mdat_header;
//extern int num_pat;
//extern int num_ts;
