
#include "AHX-Sources/AHX/AHX.h"
#include "../../player/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

/*
typedef struct HippoPlaybackPlugin
{
	int version;
	const char* (*info)(void* userData);
	const char* (*trackInfo)(void* userData);
	const char* (*supportedExtensions)(void* userData);
	int (*create)(void* userData);
	int (*destroy)(void* userData);
	int (*open)(void* userData, const char* buffer);
	int (*close)(void* userData);
	int (*readData)(void* userData, void* dest, int size);
	int (*seek)(void* userData, int ms);

	void* userData;

} HippoPlaybackPlugin;
*/

static AHXPlayer g_ahxPlayer;
static int Frequency = 44100, Bits = 16, Frames = 2, NrBlocks = 4, Hz = 50, MixLen = 10;
static float Boost = 1.0f;
static unsigned int g_mixingOffset;
int* g_mixingBuffer;
short* g_mixTemp;
static int VolumeTable[65][256];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* AhxInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* AhxTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* AhxSupportedExtensions(void* userData)
{
	return "ahx";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AhxCreate(void* userData)
{
	g_mixingBuffer = (int*)malloc(1000 * 1024 * 1024);
	g_mixTemp = (short*)g_mixingBuffer; 
	memset(g_mixingBuffer, 0, 100 * 1024 * 1024);
	AHXPlayer* player = (AHXPlayer*)userData;
	player->Init(0);

	for (int i = 0; i < 65; i++)
		for (int j = -128; j < 128; j++)
			VolumeTable[i][j+128] = (int)(i * j * Boost) / 64;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AhxDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AhxOpen(void* userData, const char* buffer)
{
	AHXPlayer* player = (AHXPlayer*)userData;
	return player->LoadSong((char*)buffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AhxClose(void* userData)
{
	return 0;
}

#define min(a,b)  (((a) < (b)) ? (a) : (b))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void MixChunk(AHXPlayer* Player, int NrSamples, int** mb)
{
	static int pos[4] = { 0, 0, 0, 0 };
	const bool Oversampling = false;

	for(int v = 0; v < 4; v++) {
		if(Player->Voices[v].VoiceVolume == 0) continue;
		float freq = Period2Freq(Player->Voices[v].VoicePeriod);
		int delta = (int)(freq * (1 << 16) / Frequency);
		int samples_to_mix = NrSamples;
		int mixpos = 0;
		while(samples_to_mix) {
			if(pos[v] > (0x280 << 16)) pos[v] -= 0x280 << 16;
			int thiscount = min(samples_to_mix, ((0x280 << 16)-pos[v]-1) / delta + 1);
			samples_to_mix -= thiscount;
			int* VolTab = &VolumeTable[Player->Voices[v].VoiceVolume][128];
			//INNER LOOP
			if(Oversampling) {
				for(int i = 0; i < thiscount; i++) {
					int offset = pos[v] >> 16;
					int sample1 = VolTab[Player->Voices[v].VoiceBuffer[offset]];
					int sample2 = VolTab[Player->Voices[v].VoiceBuffer[offset+1]];
					int frac1 = pos[v] & ((1 << 16) - 1);
					int frac2 = (1 << 16) - frac1;
					(*mb)[mixpos++] += ((sample1 * frac2) + (sample2 * frac1)) >> 16;
					pos[v] += delta;
				}
			} else {
				for(int i = 0; i < thiscount; i++) {
					(*mb)[mixpos++] += VolTab[Player->Voices[v].VoiceBuffer[pos[v] >> 16]];
					pos[v] += delta;
				}
			}
		} // while
	} // v = 0-3
	*mb += NrSamples;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOW_CLIP16     -0x8000
#define HI_CLIP16       0x7FFF
#define LOW_CLIP8      -0x80
#define HI_CLIP8        0x7F

static int AhxReadData(void* userData, void* dest, int size)
{
	AHXPlayer* player = (AHXPlayer*)userData;
	float* newDest = (float*)dest;

	for(int i = 0; i < NrBlocks; i++) 
	{
		int NrSamples = Frequency / Hz / player->Song.SpeedMultiplier;
		//int* mb = g_mixingBuffer;
		
		//memset(MixingBuffer, 0, MixLen*Frequency/Hz*sizeof(int));
		for(int f = 0; f < MixLen*player->Song.SpeedMultiplier /* MixLen = # frames */; f++) {
			player->PlayIRQ();
			MixChunk(player, NrSamples, &g_mixingBuffer);
		} // frames
	}

	static float temp = 0.0f;
	temp += 0.001f;

	for(int s = 0, t; s < size/4; s++, t += 2) 
	{
		int thissample = g_mixTemp[1 + t] << 6; // 16 bit
		//thissample = LOW_CLIP16 ? LOW_CLIP16 : thissample > HI_CLIP16 ? HI_CLIP16 : thissample;
		newDest[s] = (((float)thissample * 1.0f / 32767.0f));
		//printf("%f\n", newDest[s]);
	}

	//printf("[%d] %p %p\n",g_mixTemp[0], g_mixTemp, g_mixingBuffer); 

	//memcpy(dest, g_mixTemp, size); 
	g_mixTemp += size * 2;

	usleep(Frames*(1000/Hz));

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int AhxSeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_ahxPlugin = 
{
	1,
	AhxInfo,
	AhxTrackInfo,
	AhxSupportedExtensions,
	AhxCreate,
	AhxDestroy,
	AhxOpen,
	AhxClose,
	AhxReadData,
	AhxSeek,
	&g_ahxPlayer
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void initPlugin()
{
	g_ahxPlugin.version = 1;
	g_ahxPlugin.userData = &g_ahxPlayer;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	HippoPlaybackPlugin* AhxPlugin_getPlugin()
	{
		return &g_ahxPlugin;
	}
}

