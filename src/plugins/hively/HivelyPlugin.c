
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hivelyInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hivelyTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** hivelySupportedExtensions(void* userData)
{
	static const char* supportedFomats[] =
	{
		"ahx",
		"hvl",
		0,
	};

	return supportedFomats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* hivelyCreate()
{
	// TODO: supply custom allocator

	void* replayerData = malloc(sizeof(struct HivelyReplayerData));
	memset(replayerData, 0, sizeof(struct HivelyReplayerData));

	hvl_InitReplayer();

	return replayerData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelyDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelyOpen(void* userData, const char* buffer)
{
	static uint8_t tempData[64000];

	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc

	FILE* file = fopen(buffer, "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(tempData, size, 1, file);
	fclose(file);

	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)userData;	
	replayerData->tune = hvl_load_ahx(tempData, size, 0, 44100);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelyClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelyReadData(void* userData, void* dest, int size)
{
	int8_t* newDest = (int8_t*)dest;

	// TODO: Support more than one tune

	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)userData;	
	int decodeSize = hvl_DecodeFrame(replayerData->tune, newDest, &newDest[2], 4);

	if (decodeSize != size)
	{
		printf("missmatching decode size %d %d\n", decodeSize, size);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelySeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hivelyFrameSize(void* userData)
{
	return 44100 * sizeof(uint16_t) * 2 / 50;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hivelyPlugin = 
{
	1,
	hivelyInfo,
	hivelyTrackInfo,
	hivelySupportedExtensions,
	hivelyCreate,
	hivelyDestroy,
	hivelyOpen,
	hivelyClose,
	hivelyReadData,
	hivelySeek,
	hivelyFrameSize,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_hivelyPlugin;
}


