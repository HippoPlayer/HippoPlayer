/*
 * player.c
 * TFMX player engine.
 * jhp 26Mar96
 */

#include "tfmx_player.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "tfmx.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

#define NOTSUPPORTED fprintf(stderr, "Found code %08x at step %04x in macro %02x", x.l, c->MacroStep - 1, c->MacroNum)

#define MAYBEWAIT                  \
    if (c->NewStyleMacro == 0x0) { \
        c->NewStyleMacro = 0xFF;   \
        break;                     \
    } else {                       \
        return;                    \
    }

int notevals[] = {0x6AE, 0x64E, 0x5F4, 0x59E, 0x54D, 0x501, 0x4B9, 0x475, 0x435, 0x3F9, 0x3C0, 0x38C, 0x358,
                  0x32A, 0x2FC, 0x2D0, 0x2A8, 0x282, 0x25E, 0x23B, 0x21B, 0x1FD, 0x1E0, 0x1C6, 0x1AC, 0x194,
                  0x17D, 0x168, 0x154, 0x140, 0x12F, 0x11E, 0x10E, 0x0FE, 0x0F0, 0x0E3, 0x0D6, 0x0CA, 0x0BF,
                  0x0B4, 0x0AA, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071, 0x0D6, 0x0CA, 0x0BF, 0x0B4,
                  0x0AA, 0x0A0, 0x097, 0x08F, 0x087, 0x07F, 0x078, 0x071, 0x0D6, 0x0CA, 0x0BF, 0x0B4};


/* 4 lowest b2 bits hold voice number ?
 * higher b2 bits hold various things
 */
static void player_NotePort(TfmxState* state, U32 note) {
    UNI uni;
    struct Cdb* c;

    uni.l = note;
    c = &state->cdb[uni.b.b2 & (state->multimode ? 7 : 3)];

    if (uni.b.b0 == 0xFC) { /* lock */
        c->SfxFlag = uni.b.b1;
        c->SfxLockTime = uni.b.b3;
        return;
    }
    if (c->SfxFlag)
        return;
    if (uni.b.b0 < 0xC0) /* means normal note with finetune/macro ? */
    {
        /* seems b0:note, b1:macro, b2:velocity, b3:finetune */
        c->Finetune = (int)uni.b.b3;
        c->Velocity = (uni.b.b2 >> 4) & 0xF;
        c->PrevNote = c->CurrNote;
        c->CurrNote = uni.b.b0;
        c->ReallyWait = 1;
        c->NewStyleMacro = 0xFF;
        c->MacroNum = uni.b.b1;
        c->MacroPtr = state->macros[c->MacroNum];
        c->MacroStep = c->EfxRun = c->MacroWait = 0;
        c->KeyUp = 1;
        c->Loop = -1;
        c->MacroRun = -1;
    } else if (uni.b.b0 < 0xF0) /* means note with portamento ? */
    {
        /* seems b0:note, b1:portamento reset, b2:unused, b3:portamento rate */
        c->PortaReset = uni.b.b1;
        c->PortaTime = 1;
        if (!c->PortaRate)
            c->PortaPer = c->DestPeriod;
        c->PortaRate = uni.b.b3;
        c->CurrNote = uni.b.b0 & 0x3F;
        c->DestPeriod = notevals[c->CurrNote];
    } else
        switch (uni.b.b0) {
            case 0xF7: /* enve */
                c->EnvRate = uni.b.b1;
                c->EnvReset = c->EnvTime = (uni.b.b2 >> 4) + 1;
                c->EnvEndvol = uni.b.b3;
                break;
            case 0xF6: /* vibr */
                c->VibReset = uni.b.b1 & 0xFE;
                c->VibTime = c->VibReset / 2;
                c->VibWidth = uni.b.b3;
                c->VibFlag = 1; /* ?! */
                c->VibOffset = 0;
                break;
            case 0xF5: /* kup^ */
                c->KeyUp = 0;
                break;
        }
}

static int player_LoopOff(TfmxState* state, struct Hdb* hw) {
    return 1;
}

static int player_LoopOn(TfmxState* state, struct Hdb* hw) {
    if (!hw->c)
        return 1;
    if (hw->c->WaitDMACount--)
        return 1;
    hw->loop = &player_LoopOff;
    hw->c->MacroRun = 0xFF;
    return 1;
}

static void player_RunMacro(TfmxState* state, struct Cdb* c) {
    UNI x;
    register int a;
    /*int b;*/
    c->MacroWait = 0;
loop:
    x.l = ntohl(state->mdat_editbuf[c->MacroPtr + (c->MacroStep++)]);
    a = x.b.b0;
    x.b.b0 = 0;
    DEBUG(3) {
        printf("%02x:%02x:%02x:%02x\n", a, x.b.b1, x.b.b2, x.b.b3);
        fflush(stdout);
    }
    switch (a) {
        case 0: /* dmaoff+reset */
            DEBUG(3) puts("DMAOff+Reset");
            c->EnvReset = c->VibReset = /*c->ArpRun=c->SIDSize=*/c->PortaRate = c->AddBeginTime = 0;

            /* seems gemx was always 0 */
            /*  		if (gemx) { */
            /*  			if (x.b.b2) */
            /*  				c->CurVol=x.b.b3; */
            /*  			else */
            /*  				c->CurVol=x.b.b3+(c->Velocity)*3; */
            /*  		} */
        case 0x13: /* dmaoff */
            DEBUG(3) puts("DMAOff");
            c->hw->loop = &player_LoopOff;

            if (!x.b.b1) {
                c->hw->mode = 0;
                if (c->NewStyleMacro) { /* Patch: S. Ohlsson */
                    c->hw->SampleStart = (S8*)&state->smplbuf[c->SaveAddr];
                    c->hw->SampleLength = 1;
                    c->hw->sbeg = c->hw->SampleStart;
                    c->hw->slen = 1;
                    c->hw->pos = 0;
                }
                break;
            } else {
                c->hw->mode |= 4;
                c->NewStyleMacro = 0;
                return;
            }
        case 0x1: /* dma on */
            DEBUG(3) puts("DMAOn");
            c->EfxRun = x.b.b1;
            c->hw->mode = 1;
            if (!c->NewStyleMacro) {
                c->hw->SampleStart = (S8*)&state->smplbuf[c->SaveAddr];
                // c->hw->_SampleLength=(c->SaveLen)?c->SaveLen<<1:131072;
                // TODO: Investigate
                c->hw->SampleLength = (c->SaveLen) ? c->SaveLen << 1 : 0;
                c->hw->sbeg = c->hw->SampleStart;
                c->hw->slen = c->hw->SampleLength;
                c->hw->pos = 0;
                c->hw->mode |= 2;
                break;
            } else {
                break;
            }
        case 0x2: /* setbegin */
            DEBUG(3) puts("SetBegin");
            c->AddBeginTime = 0;
            c->SaveAddr = c->CurAddr = x.l;

            /* The code above has a tendency to cause references outside the valid range
             * smplbuf -> smplbuf+fileSize when corrupt SMPL files are used
             * with f.ex. Turrican 3.
             */
            break;
        case 0x11: /* addbegin */
            DEBUG(3) printf("AddBegin - time=%02x, delta=%04x", x.b.b1, (int)x.w.w1);

            /*
              if(((S16)x.w.w1) < 0) {
              printf("AddBegin - time=%02x, delta=%04x", x.b.b1,(int)x.w.w1);
              printf("Neg. AddBegin offset = (S16)0x%x = 0x%x!\n",x.w.w1,(S16)x.w.w1);
              printf("Loop = %d, CurAddr=%ld\n",c->Loop, c->CurAddr);
              }
            */
            c->AddBeginTime = c->AddBeginReset = x.b.b1;
            a = c->CurAddr + (c->AddBegin = (S16)x.w.w1);
            /*		if (c->SIDSize)
                            c->SIDSrcSample=c->CurAddr=a;
                            else
            */
            c->CurAddr = c->SaveAddr = a;

            break;
        case 0x3: /* setlen */
            DEBUG(3) puts("SetLen");
            c->SaveLen = c->CurrLength = x.w.w1;
            break;
        case 0x12: /* addlen */
            DEBUG(3) puts("AddLen");
            c->CurrLength += x.w.w1;
            a = c->CurrLength;
            /*		if (c->SIDSize)
                            c->SIDSrcLength=a;
                            else*/
            c->SaveLen = a;
            break;
        case 0x4:
            DEBUG(3) puts("Wait");
            if (x.b.b1 & 0x01) {
                if (c->ReallyWait++)
                    return;
            }
            c->MacroWait = x.w.w1;
            MAYBEWAIT;
        case 0x1A:
            DEBUG(3) puts("Wait on DMA");
            c->hw->loop = &player_LoopOn;
            c->hw->c = c;
            c->WaitDMACount = x.w.w1;
            c->MacroRun = 0;
            /*		return;*/
            MAYBEWAIT;
        case 0x1C: /* note split */
            DEBUG(3) puts("Splitnote");
            if (c->CurrNote > x.b.b1)
                c->MacroStep = x.w.w1;
            break;
        case 0x1D: /* vol split */
            DEBUG(3) puts("Splitvol");
            if (c->CurVol > x.b.b1)
                c->MacroStep = x.w.w1;
            break;
#if 0
	case 0x1B: /* random play */
	case 0x1E: /* random limit */
#endif
        case 0x10: /* loop key up */
            DEBUG(3) puts("Loop key up");
            if (!c->KeyUp)
                break;
        case 0x5: /* loop */
            DEBUG(3) puts("Loop");
            if (!(c->Loop--))
                break;
            else if (c->Loop < 0)
                c->Loop = x.b.b1 - 1;
            c->MacroStep = x.w.w1;
            break;
        case 0x7: /* stop */
            DEBUG(3) puts("STOP");
            c->MacroRun = 0;
            return;
        case 0xD: /* add volume */
            DEBUG(3) puts("Addvolume");
            if (x.b.b2 != 0xFE) {
                c->CurVol = (c->Velocity * 3) + x.b.b3;
                break;
            }
            NOTSUPPORTED;
            break;
            /*			c->CurVol=c->Velocity*3+x.b.b3;
                                    PutNote */
        case 0xE: /* set volume */
            DEBUG(3) puts("Setvolume");
            if (x.b.b2 != 0xFE) {
                c->CurVol = x.b.b3;
                break;
            }
            NOTSUPPORTED;
            break;
        case 0x21: /* start macro */
            DEBUG(3) puts("Play macro");
            x.b.b0 = c->CurrNote;
            x.b.b2 |= c->Velocity << 4;
            player_NotePort(state, x.l);
            break;
        case 0x1F: /* set prev note */
            DEBUG(3) puts("AddPrevNote");
            a = c->PrevNote;
            goto SetNote;
        case 0x8:
            DEBUG(3) puts("Addnote");
            a = c->CurrNote;
            goto SetNote;
        case 0x9:
            DEBUG(3) puts("SetNote");
            a = 0;
        SetNote:
            a = (notevals[(a + x.b.b1) & 0x3F] * (0x100 + c->Finetune + (S8)x.b.b3)) / 256;
            c->DestPeriod = a;
            if (!c->PortaRate)
                c->CurPeriod = a;
            MAYBEWAIT;
        case 0x17: /* setperiod */
            DEBUG(3) puts("Setperiod");
            c->DestPeriod = x.w.w1;
            if (!c->PortaRate)
                c->CurPeriod = x.w.w1;
            break;
        case 0xB: /* portamento */
            DEBUG(3) puts("Portamento");
            c->PortaReset = x.b.b1;
            c->PortaTime = 1;
            if (!c->PortaRate)
                c->PortaPer = c->DestPeriod;
            c->PortaRate = x.w.w1;
            break;
        case 0xC: /* vibrato */
            DEBUG(3) puts("Vibrato");
            c->VibReset = x.b.b1;
            c->VibTime = c->VibReset / 2;
            c->VibWidth = x.b.b3;
            c->VibFlag = 1;
            if (!c->PortaRate) {
                c->CurPeriod = c->DestPeriod;
                c->VibOffset = 0;
            }
            break;
        case 0xF: /* envelope */
            DEBUG(3) puts("Envelope");
            c->EnvReset = c->EnvTime = x.b.b2;
            c->EnvEndvol = x.b.b3;
            c->EnvRate = x.b.b1;
            break;
        case 0xA: /* reset */
            DEBUG(3) puts("Reset efx");
            c->EnvReset = c->VibReset = /*c->ArpRun=c->SIDSize=*/c->PortaRate = c->AddBeginTime = 0;
            break;
        case 0x14: /* wait key up */
            DEBUG(3) puts("Wait key up");
            if (!c->KeyUp)
                c->Loop = 0;
            if (!c->Loop) {
                c->Loop = -1;
                break;
            }
            if (c->Loop == -1)
                c->Loop = x.b.b3 - 1;
            else
                c->Loop--;
            c->MacroStep--;
            return;
        case 0x15: /* go sub */
            DEBUG(3) puts("Gosub patt");
            c->ReturnPtr = c->MacroPtr;
            c->ReturnStep = c->MacroStep;
        case 0x6: /* cont */
            DEBUG(3) puts("Continue");
            c->MacroPtr = c->MacroNum = state->macros[x.b.b1];
            c->MacroStep = x.w.w1;
            c->Loop = 0xFFFF;
            break;
        case 0x16: /* return sub */
            DEBUG(3) puts("Returnpatt");
            c->MacroPtr = c->ReturnPtr;
            c->MacroStep = c->ReturnStep;
            break;
        case 0x18: /* sampleloop */
            DEBUG(3) puts("Sampleloop");
            c->SaveAddr += (x.w.w1 & 0xFFFE);
            c->SaveLen -= x.w.w1 / 2;
            c->CurrLength = c->SaveLen;
            c->CurAddr = c->SaveAddr;
            break;
        case 0x19: /* oneshot */
            DEBUG(3) puts("One-shot");
            c->AddBeginTime = 0;
            c->SaveAddr = c->CurAddr = 0;
            c->SaveLen = c->CurrLength = 1;
            break;
        case 0x20: /* cue */
            DEBUG(3) puts("Cue");
            /*  	    idb.Cue[x.b.b1 & 0x03] = x.w.w1; */
            break;
        case 0x31: /* turrican 3 title - we can safely ignore */
            break;
        default:
            NOTSUPPORTED;
            break;
            c->MacroRun = 0;
            return;
    }
    goto loop;
}

static void player_DoEffects(TfmxState* state, struct Cdb* c) {
    int a;

    if (c->EfxRun < 0)
        return;
    if (!c->EfxRun) {
        c->EfxRun = 1;
        return;
    }
    if (c->AddBeginTime) {
        c->CurAddr += c->AddBegin;
        /*		if (c->SIDSize)
                        c->SIDSrcSample=c->CurAddr;
                        else*/
        c->SaveAddr = c->CurAddr;
        c->AddBeginTime--;
        if (!c->AddBeginTime) {
            c->AddBegin = -c->AddBegin;
            c->AddBeginTime = c->AddBeginReset;
        }
    }

    // TODO: Invistigate
    // if(c->CurAddr < 0)
    // c->AddBegin=c->AddBeginTime=c->AddBeginReset=c->CurAddr = 0;

    /*
      if (c->SIDSize) {
      fputs("SID not supported\n",stderr);
      c->SIDSize=0;
      }
    */
    if (c->VibReset) {
        c->VibOffset += c->VibWidth;
        a = (c->DestPeriod * (0x800 + c->VibOffset)) / 2048;
        if (!c->PortaRate)
            c->CurPeriod = a;
        if (!(--c->VibTime)) {
            c->VibTime = c->VibReset;
            c->VibWidth = -c->VibWidth;
        }
    }
    if ((c->PortaRate) && ((--c->PortaTime) == 0)) {
        c->PortaTime = c->PortaReset;
        if (c->PortaPer > c->DestPeriod) {
            a = (c->PortaPer * (256 - c->PortaRate) - 128) / 256;
            if (a <= c->DestPeriod)
                c->PortaRate = 0;
        } else if (c->PortaPer < c->DestPeriod) {
            a = (c->PortaPer * (256 + c->PortaRate)) / 256;
            if (a >= c->DestPeriod)
                c->PortaRate = 0;
        } else
            c->PortaRate = 0;
        if (!c->PortaRate)
            a = c->DestPeriod;
        c->PortaPer = c->CurPeriod = a;
    }
    if ((c->EnvReset) && (!(c->EnvTime--))) {
        c->EnvTime = c->EnvReset;
        if (c->CurVol > c->EnvEndvol) {
            if (c->CurVol < c->EnvRate)
                c->EnvReset = 0;
            else
                c->CurVol -= c->EnvRate;
            if (c->EnvEndvol > c->CurVol)
                c->EnvReset = 0;
        } else if (c->CurVol < c->EnvEndvol) {
            c->CurVol += c->EnvRate;
            if (c->EnvEndvol < c->CurVol)
                c->EnvReset = 0;
        }
        if (!c->EnvReset) {
            c->EnvReset = c->EnvTime = 0;
            c->CurVol = c->EnvEndvol;
        }
    }
    /*	if (c->ArpRun) {
            fputs("Arpeggio/randomplay not supported\n",stderr);
            c->ArpRun=0;
            }
    */
    if ((state->mdb.FadeSlope) && ((--state->mdb.FadeTime) == 0)) {
        state->mdb.FadeTime = state->mdb.FadeReset;
        state->mdb.MasterVol += state->mdb.FadeSlope;
        if (state->mdb.FadeDest == state->mdb.MasterVol)
            state->mdb.FadeSlope = 0;
    }
}

static void player_DoMacro(TfmxState* state, int cc) {
    struct Cdb* c = &state->cdb[cc];
    int a;

    /* locking */
    if (c->SfxLockTime >= 0)
        c->SfxLockTime--;
    else
        c->SfxFlag = c->SfxPriority = 0;

    a = c->SfxCode;
    if (a) {
        c->SfxFlag = c->SfxCode = 0;
        player_NotePort(state, a);
        c->SfxFlag = c->SfxPriority;
    }
    DEBUG(3)
    printf("%01x:\t", cc);
    if ((c->MacroRun) && !(c->MacroWait--))
        player_RunMacro(state, c);
    DEBUG(3) puts("");
    player_DoEffects(state, c);
    /* has to be here because of if(efxrun=1) */
    c->hw->delta = c->CurPeriod ? (3579545 * 512) / (c->CurPeriod * state->outRate / 32) : 0;
    c->hw->SampleStart = (S8*)&state->smplbuf[c->SaveAddr];

    // TODO: Invistigate
    // c->hw->SampleLength = c->SaveLen ? c->SaveLen * 2 : 131072;
    c->hw->SampleLength = c->SaveLen ? c->SaveLen * 2 : 0;
    if ((c->hw->mode & 3) == 1) {
        c->hw->sbeg = c->hw->SampleStart;
        c->hw->slen = c->hw->SampleLength;
    }
    c->hw->vol = (c->CurVol * state->mdb.MasterVol) / 64;
}

static void player_DoAllMacros(TfmxState* state) {
    player_DoMacro(state, 0);
    player_DoMacro(state, 1);
    player_DoMacro(state, 2);

    if (state->multimode) {
        player_DoMacro(state, 4);
        player_DoMacro(state, 5);
        player_DoMacro(state, 6);
        player_DoMacro(state, 7);
    } /* else -- DoMacro(3) should always run so fade speed is right */
    player_DoMacro(state, 3);
}

static void player_ChannelOff(TfmxState* state, int i) {
    struct Cdb* c;

    c = &state->cdb[i & 0xF];
    if (!c->SfxFlag) {
        c->hw->mode = 0;
        c->AddBeginTime = c->AddBeginReset = c->MacroRun =
            /*c->SIDSize=c->ArpRun=*/0;
        c->NewStyleMacro = 0xFF;
        c->SaveAddr = c->CurVol = c->hw->vol = 0;
        c->SaveLen = c->CurrLength = 1;
        c->hw->loop = &player_LoopOff;
        c->hw->c = c;
    }
}

static void player_DoFade(TfmxState* state, int sp, int dv) {
    state->mdb.FadeDest = dv;
    if (!(state->mdb.FadeTime = state->mdb.FadeReset = sp) || (state->mdb.MasterVol == sp)) {
        state->mdb.MasterVol = dv;
        state->mdb.FadeSlope = 0;
        return;
    }
    state->mdb.FadeSlope = (state->mdb.MasterVol > state->mdb.FadeDest) ? -1 : 1;
}

// Track-step commands:
// command 	parameter 	command
// function
// EFFE 0000 	none 	STOP 	The song is stopped.
// EFFE 0001 	yyyy 	LOOP 	The song loops to track-step line yyyy.
// EFFE 0002 	yyyy 	SPEED 	The song speed is changed to yyyy.
// EFFE 0003 	yyyy 	TIME SHARE * 	Reduce the processing time of the player on (yyyy = 0001) or off (yyyy = 0000).
// * This command only works with TFMX music routines without support of the 7-voice mode.
// EFFE 0003 	00yy 00zz 	7-VOICE ** 	Playback rate of the song in yy kHz, fine adjustment of the song speed by zz.
// ** This command only works with TFMX music routines that support 7-voice mode.
// EFFE 0004 	00yy 00zz 	FADE 	The volume of the song is faded to zz with a speed of yy.

static void player_GetTrackStep(TfmxState* state) {
    U16* l;
    int x, y;
loop:

    /*printf("GetTrackStep, loops = %d    ", loops);*/

    if (state->loops > 0) {
        if ((state->pdb.CurrPos == state->pdb.FirstPos) && (!(--state->loops))) {
            state->mdb.PlayerEnable = 0;
            /*printf("Disabled mdb.PlayerEnable\n");*/
            return;
        }
    }

    l = (U16*)&state->mdat_editbuf[state->mdat_header.trackstart + (state->pdb.CurrPos * 4)];

    if (state->printinfo) {
        printf("%04x:", state->pdb.CurrPos);
        for (x = 0; x < 8; x++)
            printf("%04x ", l[x]);
        DEBUG(2) { printf("tempo=%d pre=%d jif=%d", 0x1B51F8 / state->mdb.CIASave, state->pdb.Prescale, state->jiffies); }
        puts("");
    }

    state->jiffies = 0;
    if ((l[0]) == 0xEFFE) {
        switch (l[1]) {
            case 0: /* stop */
                state->mdb.PlayerEnable = 0;
                return;
            case 1: /* loop */
                /*if (loops>0) {
                  if (!(--loops)) {
                  mdb.PlayerEnable=0;
                  return;
                  }
                  }*/
                if (!(state->mdb.TrackLoop--)) {
                    state->mdb.TrackLoop = -1;
                    state->pdb.CurrPos++;
                    goto loop;
                } else if (state->mdb.TrackLoop < 0)
                    state->mdb.TrackLoop = l[3];
                state->pdb.CurrPos = l[2];
                goto loop;
            case 2: /* speed */
                state->mdb.SpeedCnt = state->pdb.Prescale = l[2];
                if (!(l[3] & 0xF200) && (x = (l[3] & 0x1FF) > 0xF))
                    state->mdb.CIASave = state->eClocks = 0x1B51F8 / x;
                state->pdb.CurrPos++;
                goto loop;
            case 3: /* timeshare */
                if (!((x = l[3]) & 0x8000)) {
                    x = ((char)x) < -0x20 ? -0x20 : (char)x;
                    state->mdb.CIASave = state->eClocks = (14318 * (x + 100)) / 100;
                    state->multimode = 1;
                } /* else state->multimode=0;*/
                state->pdb.CurrPos++;
                goto loop;
            case 4: /* fade */
                player_DoFade(state, l[2] & 0xFF, l[3] & 0xFF);
                state->pdb.CurrPos++;
                goto loop;
            default:
                fprintf(stderr, "EFFE %04x in trackstep\n", l[1]);
                state->pdb.CurrPos++;
                goto loop;
        }
    } else {
        for (x = 0; x < 8; x++) {
            state->pdb.p[x].PXpose = (int)(l[x] & 0xff);
            state->pdb.p[x].PNum = l[x] / 256;
            y = state->pdb.p[x].PNum;
            if (y < 0x80) {
                state->pdb.p[x].PStep = 0;
                state->pdb.p[x].PWait = 0;
                state->pdb.p[x].PLoop = 0xFFFF;
                state->pdb.p[x].PAddr = state->patterns[y];
            }
        }
    }
}

/* pp range : 0 .. 7 */
static int player_DoTrack(TfmxState* state, struct Pdb* p, int pp) {
    UNI x;
    unsigned char t; /*was: int*/

	// Valid pattern numbers for the track page are 0x00 to 0x7F.
	// This corresponds to a maximum number of 128 patterns.
	// In addition, there are special pattern commands that apply to a single track,
	// and track-step commands that affect the entire song.

	// Pattern commands:
    // 0x80 - Continue playing the previous pattern
    // 0xFE - Deletes the audio channel specified in the transpose value
    // 0xFF - empty pattern

    if (p->PNum == 0xFE) {
        p->PNum++;
        player_ChannelOff(state, p->PXpose);
        return (0);
    }

    if (!p->PAddr)
        return (0);
    if (p->PNum >= 0x90)
        return (0);
    if (p->PWait--)
        return (0);

    while (1) {
    loop:
        x.l = ntohl(state->mdat_editbuf[p->PAddr + p->PStep++]);
        t = x.b.b0;

        /*printf("%x: %02x:%02x:%02x:%02x (%04x)\n",pp,t,x.b.b1,x.b.b2,x.b.b3,jiffies);*/

        if (t < 0xF0) {
            if ((t & 0xC0) == 0x80) {
                p->PWait = x.b.b3;
                x.b.b3 = 0;
            }

            x.b.b0 = ((t + p->PXpose) & 0x3F);

            if ((t & 0xC0) == 0xC0)
                x.b.b0 |= 0xC0;

            player_NotePort(state, x.l);

            if ((t & 0xC0) == 0x80)
                return (0);
            goto loop;
        }
        switch (t & 0xF) {
            case 15: /* NOP */
                break;
            case 0: /* End */
                p->PNum = 0xFF;
                state->pdb.CurrPos = (state->pdb.CurrPos == state->pdb.LastPos) ? state->pdb.FirstPos : state->pdb.CurrPos + 1;
                player_GetTrackStep(state);
                return (1);
            case 1:
                if (!(p->PLoop)) {
                    p->PLoop = 0xFFFF;
                    break;
                } else if (p->PLoop == 0xFFFF) /* FF --'ed */
                    p->PLoop = x.b.b1;
                p->PLoop--;
                p->PStep = x.w.w1;
                break;
            case 8: /* GsPt */
                p->PRoAddr = p->PAddr;
                p->PRoStep = p->PStep;
                /* fall through to... */
            case 2: /* Cont */
                p->PAddr = state->patterns[x.b.b1];
                p->PStep = x.w.w1;
                break;
            case 3: /* Wait */
                p->PWait = x.b.b1;
                return (0);
            case 14: /* StCu */
                state->mdb.PlayPattFlag = 0;
            case 4: /* Stop */
                p->PNum = 0xFF;
                return (0);
            case 5:  /* Kup^ */
            case 6:  /* Vibr */
            case 7:  /* Enve */
            case 12: /* Lock */
                player_NotePort(state, x.l);
                break;
            case 9: /* RoPt */
                p->PAddr = p->PRoAddr;
                p->PStep = p->PRoStep;
                break;
            case 10: /* Fade */
                player_DoFade(state, x.b.b1, x.b.b3);
                break;
            case 13: /* Cue */
                     /*  		idb.Cue[ x.b.b1 & 0x03] = x.w.w1; */
                break;
            case 11: /* PPat */
                t = x.b.b2 & 0x07;
                state->pdb.p[t].PNum = x.b.b1;
                state->pdb.p[t].PAddr = state->patterns[x.b.b1];
                state->pdb.p[t].PXpose = x.b.b3;
                state->pdb.p[t].PStep = 0;
                state->pdb.p[t].PWait = 0;
                state->pdb.p[t].PLoop = 0xFFFF;
                break;
        }
    }
}

static void player_DoTracks(TfmxState* state) {
    int i;

    state->jiffies++;
    if (!state->mdb.SpeedCnt--) {
        state->mdb.SpeedCnt = state->pdb.Prescale;
        for (i = 0; i < 8; i++) {
            DEBUG(3) printf("%01x:", i);
            if (player_DoTrack(state, &state->pdb.p[i], i)) {
                i = -1;
                continue;
            }
        }
    }
}

static void player_AllOff(TfmxState* state) {
    int x;
    struct Cdb* c;
    state->mdb.PlayerEnable = 0;
    for (x = 0; x < 8; x++) {
        c = &state->cdb[x];
        c->hw = &state->hdb[x];
        c->hw->c = c; /* wait on dma */
        state->hdb[x].mode = 0;
        c->MacroWait = c->MacroRun = c->CurVol = c->SfxFlag = c->SfxCode = c->SaveAddr = 0;
        state->hdb[x].vol = 0;
        c->Loop = c->NewStyleMacro = c->SfxLockTime = -1;
        c->hw->sbeg = c->hw->SampleStart = (S8*)state->smplbuf;
        c->hw->SampleLength = c->hw->slen = c->SaveLen = 2;
        c->hw->loop = &player_LoopOff;
    }
}

void player_tfmxIrqIn(TfmxState* state) {
    if (!state->mdb.PlayerEnable)
        return;
    player_DoAllMacros(state);
    if (state->mdb.CurrSong >= 0)
        player_DoTracks(state);
}

void player_TfmxInit(TfmxState* state) {
    int x;
    player_AllOff(state);
    for (x = 0; x < 8; x++) {
        state->hdb[x].c = &state->cdb[x];
        state->pdb.p[x].PNum = 0xFF;
        state->pdb.p[x].PAddr = 0;
        player_ChannelOff(state, x);
    }
    return;
}

void player_StartSong(TfmxState* state, int song, int mode) {
    int x;

    /* Prevent # of loops to play from changing during init */
    int oldLoopsVal = state->loops;

    /* No limitations on loops & jumps during init */
    state->loops = -1;

    state->mdb.PlayerEnable = 0; /* sort of locking mechanism */
    state->mdb.MasterVol = 0x40;
    state->mdb.FadeSlope = 0;
    state->mdb.TrackLoop = -1;
    state->mdb.PlayPattFlag = 0;
    state->mdb.CIASave = state->eClocks = 14318; /* assume 125bpm, NTSC timing */
    if (mode != 2) {
        state->pdb.CurrPos = state->pdb.FirstPos = state->mdat_header.start[song];
        state->pdb.LastPos = state->mdat_header.end[song];
        if ((x = state->mdat_header.tempo[song]) >= 0x10) {
            state->mdb.CIASave = state->eClocks = 0x1B51F8 / x;
            state->pdb.Prescale = 0;
        } else
            state->pdb.Prescale = x;
    }
    for (x = 0; x < 8; x++) {
        state->pdb.p[x].PAddr = 0;
        state->pdb.p[x].PNum = 0xFF;
        state->pdb.p[x].PXpose = 0;
        state->pdb.p[x].PStep = 0;
    }
    if (mode != 2)
        player_GetTrackStep(state);
    /* seems startPat was always -1 */
    /*  	if (startPat!=-1) { */
    /*  		pdb.CurrPos=pdb.FirstPos=startPat; */
    /*  		player_GetTrackStep(); */
    /*  		startPat=-1; */
    /*  	} */
    state->mdb.SpeedCnt = state->mdb.EndFlag = 0;
    state->mdb.PlayerEnable = 1;

    state->loops = oldLoopsVal;
}

char player_TFMXVoices(TfmxState* state) {
    U16* trkstart = (U16*)&state->mdat_editbuf[state->mdat_header.trackstart];

    /* Check for timeshare command at start of tracks */
    if (trkstart[0] == 0xeffe && trkstart[1] == 0x0003)
        return 7;
    return 4;
}

void TfmxResetBuffers(TfmxState* state);

void TFMXRewind(TfmxState* state) {
    player_TfmxInit(state);
    /* printf("StartSong, loops = %d\n", loops); */
    player_StartSong(state, state->songnum, 0);

    state->hdb[0].pos = 0;
    state->hdb[0].delta = 0x1c01;
    state->hdb[0].slen = 0x3200;
    state->hdb[0].SampleLength = 0x15be;
    state->hdb[0].sbeg = (S8*)&state->smplbuf[0x4];
    state->hdb[0].SampleStart = (S8*)&state->smplbuf[0x4 + 0x1C42];
    state->hdb[0].vol = 0x40;
    state->hdb[0].mode = 3;
    state->hdb[0].loop = &player_LoopOff;
    state->hdb[0].loopcnt = 0;
    state->hdb[0].c = NULL;

    TfmxResetBuffers(state);
}

void TFMXStop(TfmxState* state) {
    /* player_TfmxInit(); */
}

void TfmxTakedown(TfmxState* state);

void TFMXQuit(TfmxState* state) {
    if (state->smplbuf) {
        free(state->smplbuf);
        state->smplbuf = 0;
    }
    TfmxTakedown(state);
}

/* extern unsigned long blocksize,multiplier,stereo; */
/* extern int bhead=0,btail=0,bqueue=0; */

int TFMXGetSubSongs(TfmxState* state) {
    return state->nSongs;
}
int TFMXGetSubSong(TfmxState* state) {
    return state->songnum;
}

void TFMXSetSubSong(TfmxState* state, int num) {
    int x;

    /* Compensate for empty song slots */
    for (x = 0; x < num && x < 31; x++) {
        if ((state->mdat_header.start[x] > state->mdat_header.end[x]) ||
            (x > 0 && state->mdat_header.end[x] == 0L)) {
            num++;
        }
    }
    state->songnum = num;
    TFMXRewind(state);
}
