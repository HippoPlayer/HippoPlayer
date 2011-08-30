/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is DFREE software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  $Id: drv_gp32.c,v 1.2 2005/03/30 19:09:11 realtech Exp $

  Driver for output on gp32 platform 

==============================================================================*/

/*
	Written by ??????
   
    Adapted from the windows waveout driver,
	Written by Bjornar Henden <bhenden@online.no>

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

#ifdef DRV_GP32

#define NUMBUFFERS	6				/* number of buffers */
#define BUFFERSIZE	120				/* buffer size in milliseconds */

unsigned short *buffer=NULL;
unsigned short *buffer_end=NULL;

int *play_pos;
int *play_chan;
int buffer_size;
int writting_buffer=0;

#define SAMPLERATE 44100
#define BUFFER1_SIZE ((SAMPLERATE*sizeof(unsigned short)*2*BUFFERSIZE)/1000)

static BOOL GP32_IsThere(void)
{
	return 1;
}

static BOOL GP32_Init(void)
{

	md_mode|=DMODE_SOFT_MUSIC|DMODE_SOFT_SNDFX;
	GpPcmInit ( PCM_S44, PCM_16BIT );
	
	buffer_size=BUFFER1_SIZE*NUMBUFFERS;
	buffer=gp_mem_func.MikMod_malloc(buffer_size);
	gp_str_func.memset(buffer,0,buffer_size);
	
	if (!buffer) {
			_mm_errno = MMERR_OUT_OF_MEMORY;
			return 0;
	
	}
	
	GpPcmPlay((unsigned short *)buffer,buffer_size,1);
    GpPcmLock((unsigned short *)buffer,(int *)&play_chan,(unsigned int *)&play_pos);
	
	writting_buffer=0;
	
	return VC_Init();

}

void GP32_Stop(void) {
	GpPcmStop();
}

void GP32_Restart(void) {
	GpPcmPlay((unsigned short *)buffer,buffer_size,1);
    GpPcmLock((unsigned short *)buffer,(int *)&play_chan,(unsigned int *)&play_pos);
}

static void GP32_Exit(void)
{
	int n;
	GpPcmStop();
	gp_mem_func.MikMod_free(buffer);
	VC_Exit();

}

int last_data;
int next_data;

static void GP32_Update(void)
{
	ULONG done;
	int playing_buffer;
	char *writeTo;
	unsigned short *bTO;
	int samples;
	
	int top_buffer;
	
	
	playing_buffer=(*play_pos-(int)buffer)/BUFFER1_SIZE;
	
	
	
	
	
	while(writting_buffer!=playing_buffer) {
	
		writeTo=(char *)(((unsigned int)buffer)+writting_buffer*BUFFER1_SIZE);
		
		bTO=writeTo;

		done=VC_WriteBytes(writeTo,BUFFER1_SIZE);		
		
		for (samples=0;samples<(BUFFER1_SIZE/2);samples++) {
			
			//next_data=*bTO;				
			*bTO=*bTO+0x8000; // to unsigned... sdk..
			bTO++;
			//last_data=next_data;
		}
		

		
		if(!done) break;	
		writting_buffer=(writting_buffer+1)%NUMBUFFERS;
	}

}

static void GP32_PlayStop(void)
{
	//GpPcmStop();
	gp_str_func.memset(buffer,0,buffer_size);
	VC_PlayStop();

}

MIKMODAPI MDRIVER drv_gp32={
	NULL,
	"GP32 SDK Audio v0.1",
	"GP32 SDK Audio driver v0.1",
	0,255,
	"gp32",
	NULL,	
	NULL,
	GP32_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	GP32_Init,
	GP32_Exit,
	NULL,
	VC_SetNumVoices,
	VC_PlayStart,
	GP32_PlayStop,
	GP32_Update,
	NULL,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};

#else

MISSING(drv_gp32);

#endif

/* ex:set ts=4: */
