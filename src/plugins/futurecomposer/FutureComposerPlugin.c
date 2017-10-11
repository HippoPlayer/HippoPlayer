#include "../../plugin_api/HippoPlugin.h"
#include "src/fc14audiodecoder.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FCReplayerData 
{
	void* decoder; 
	void* tune;
} FCReplayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* fcInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* fcTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** fcSupportedExtensions(void* userData)
{
	static const char* supportedFomats[] =
	{
		"fc",
		0,
	};

	return supportedFomats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* fcCreate()
{
	FCReplayerData* replayer;

	// TODO: supply custom allocator

	replayer = (FCReplayerData*)malloc(sizeof(struct FCReplayerData));
	memset(replayer, 0, sizeof(struct FCReplayerData));

	replayer->decoder = fc14dec_new();
    fc14dec_mixer_init(replayer->decoder, 44100, 16, 2, 0x8000);

	return replayer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcOpen(void* userData, const char* buffer)
{
	int status;
	char* tempData; // TODO: Fix proper allocator
	FCReplayerData* replayer = (FCReplayerData*)userData;	

	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc

	FILE* file = fopen(buffer, "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	tempData = malloc(size);
	fread(tempData, size, 1, file);
	fclose(file);

    // Apply input format header check to a memory buffer. Returns: 0 = recognized data, 1 = unknown data
    status = fc14dec_detect(replayer->decoder, tempData, size);

	// 1 = unknown format
    if (status == 0)
	{
		// TODO: Signar proper error message
		printf("FCPlugin: Unable to detect proper format for %s\n", buffer);
    	return -1;
	}

	// init the tune
    status = fc14dec_init(replayer->decoder, tempData, size);

    if (status == 0)
	{
		// TODO: Signar proper error message
		printf("FCPlugin: Unable to init player for %s\n", buffer);
    	return -1;
	}

	const char* formatName = fc14dec_format_name(replayer->decoder);
	printf("FCPlugin: Format name %s\n", formatName == NULL ? "" : formatName);

    replayer->tune = tempData;

	//replayerData->tune = hvl_load_ahx(tempData, size, 0, 44100);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcReadData(void* userData, void* dest)
{
	struct FCReplayerData* replayerData = (struct FCReplayerData*)userData;	
    fc14dec_buffer_fill(replayerData->decoder, dest, 16 * 1024);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcSeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int fcFrameSize(void* userData)
{
	return 16 * 1024;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_fcPlugin = 
{
	1,
	fcInfo,
	fcTrackInfo,
	fcSupportedExtensions,
	fcCreate,
	fcDestroy,
	fcOpen,
	fcClose,
	fcReadData,
	fcSeek,
	fcFrameSize,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_fcPlugin;
}




