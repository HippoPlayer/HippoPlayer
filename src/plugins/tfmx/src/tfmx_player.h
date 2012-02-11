/*
 *
 *
 */

#define NO_SCHED

#include "tfmx.h"

struct Hdb {
    U32    pos;
    U32    delta;
    U16    slen;
    U16    SampleLength;
    S8    *sbeg;
    S8    *SampleStart;
    U8     vol;
    U8     mode;
    int    (*loop)();
    int    loopcnt;
    struct Cdb *c;
};

struct Idb {
    U16 Cue[4];
};

struct Mdb {
    char	PlayerEnable;
    char	EndFlag;
    char	CurrSong;
    U16   	SpeedCnt;
    U16 	CIASave;
    char	SongCont;
    char        SongNum;
    U16	        PlayPattFlag;
    char	MasterVol;
    char        FadeDest;
    char        FadeTime;
    char        FadeReset;
    char        FadeSlope;
    S16	        TrackLoop;
    U16	        DMAon;
    U16         DMAoff;
    U16         DMAstate;
    U16         DMAAllow;
};

struct Pdb {
    U32 PAddr;
    U8  PNum;
    S8  PXpose;
    U16 PLoop;
    U16 PStep;
    U8  PWait;
    U16 PRoAddr;
    U16 PRoStep;
};

struct Pdblk {
    U16        FirstPos;
    U16        LastPos;
    U16        CurrPos;
    U16        Prescale;
    struct Pdb p[8];
};
	
struct Cdb {
    S8  MacroRun,EfxRun;
    U8  NewStyleMacro;
    U8  PrevNote,CurrNote;
    U8  Velocity,Finetune;
    U8  KeyUp,ReallyWait;
    U32 MacroPtr;
    U16 MacroStep,MacroWait,MacroNum;
    S16 Loop;

    U32 CurAddr,SaveAddr;
    U16 CurrLength,SaveLen;

    U16 WaitDMACount;
    U16 DMABit;

    U8 EnvReset,EnvTime,EnvRate;
    S8 EnvEndvol,CurVol;

    S16 VibOffset;
    S8 VibWidth;
    U8 VibFlag,VibReset,VibTime;

    U8 PortaReset,PortaTime;
    U16 CurPeriod,DestPeriod,PortaPer;
    S16 PortaRate;

    U8 AddBeginTime,AddBeginReset;
    U16 ReturnPtr,ReturnStep;
    S32 AddBegin;

    U8 SfxFlag,SfxPriority,SfxNum;
    S16 SfxLockTime;
    U32 SfxCode;

    struct Hdb *hw;
};

char player_TFMXVoices(void);	// Return the # of voices
void player_tfmxIrqIn(void);

extern struct Pdblk pdb;
extern int nSongs;

/* one Hdb per voice. 4 voices: 0,1,2,3; 7 voices: 4,5,6,7,0,1,2 */

extern struct Hdb hdb[8];
extern struct Cdb cdb[8];
extern struct Mdb mdb;
extern int jiffies;

