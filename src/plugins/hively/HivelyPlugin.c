
#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <math.h>
#include "replayer/hvl_replay.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HivelyReplayerData
{
	struct hvl_tune* tune;
};

static struct HivelyReplayerData g_replayerData;
int8_t* g_mixingBuffer0;
int8_t* g_mixingBuffer1;
int16_t* g_mixTemp;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* HivelyInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* HivelyTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* HivelySupportedExtensions(void* userData)
{
	return ".ahx;.hvl";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelyCreate(void* userData)
{
	hvl_InitReplayer();
	g_mixingBuffer0 = (int8_t*)malloc(100 * 1024 * 1024);
	g_mixingBuffer1 = (int8_t*)malloc(100 * 1024 * 1024);
	g_mixTemp = (int16_t*)g_mixingBuffer0; 
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelyDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelyOpen(void* userData, const char* buffer)
{
	struct HivelyReplayerData* replayerData;
	static uint8_t tempData[64000];
	int size;
	FILE* file;

	file = fopen(buffer, "rb");
	fseek(file, 0, SEEK_END);
	size = (int)ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(tempData, size, 1, file);
	fclose(file);

	replayerData = (struct HivelyReplayerData*)userData;	
	replayerData->tune = hvl_load_ahx(tempData, size, 0, 44100);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelyClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelyReadData(void* userData, void* dest, int size)
{
	int16_t* newDest = (int16_t*)dest;
	int s, t;

	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)userData;	
	int decodeSize = hvl_DecodeFrame(replayerData->tune, g_mixingBuffer0, &g_mixingBuffer0[2], 4);
	int len = 44100 * sizeof( uint16_t ) * 2 / 50;;
	//printf("%d\n", len);

	for (s = 0, t = 0; s < size; s++, t += 2) 
	{
		newDest[s] = g_mixTemp[0];// * 1.0f / 32767.0f;
		g_mixTemp += 2;
	}

	//FILE* fi = fopen("/Users/daniel/dump.raw", "ab"); fwrite(g_mixTemp, size, 1, fi); fclose(fi);

	//g_mixTemp += size;

	g_mixingBuffer0 += len;
	//g_mixingBuffer1 += decodeSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int HivelySeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hivelyPlugin = 
{
	1,
	HivelyInfo,
	HivelyTrackInfo,
	HivelySupportedExtensions,
	HivelyCreate,
	HivelyDestroy,
	HivelyOpen,
	HivelyClose,
	HivelyReadData,
	HivelySeek,
	&g_replayerData
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_hivelyPlugin;
}


