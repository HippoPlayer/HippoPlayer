
#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <math.h>
#include "replayer/hvl_replay.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MikmodReplayerData
{
	HMODULE* module;
};

static struct MikmodReplayerData g_replayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* mikmodInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* mikmodTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** mikmodSupportedExtensions(void* userData)
{
	static const char* supportedFomats[] =
	{
		"669",   // Composer 669
		"amf",   // DMP Advanced Module Format
		"dsm",   // DSIK internal format
		"far",   // Farandole module
		"gdm",   // General DigiMusic module
		"imf",   // Imago Orpheus module
		"it",    // Impulse tracker
		"m15",   // 15 instrument MOD / Ultimate Sound Tracker (old M15 format)
		"med",   // Amiga MED module
		"mod",   // Generic MOD (Protracker, StarTracker, FastTracker, etc)
		"mtm",   // MTM module
		"okt",   // Oktalyzer module
		"s3m",   // Screamtracker module
		"stm",   // Screamtracker 2 module
		"stx",   // STMIK 0.2 module
		"ult",   // Ultratracker module
		"uni",   // UNIMOD - libmikmod's and APlayer's internal module format
		"xm",    // Fasttracker module
		0,
	};

	return supportedFomats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mikmodCreate()
{
	// TODO: supply custom allocator
	void* replayerData = malloc(sizeof(struct MikmodReplayerData));
	memset(replayerData, 0, sizeof(struct MikmodReplayerData));

	MikMod_RegisterDriver(&drv_nos);
	MikMod_RegisterAllLoaders();

	/*
	* Both DMODE_SOFT_MUSIC and DMODE_16BITS should be set by default,
	* so this is just for clarity. I haven't experimented with any of
	* the other flags. There are a few which are said to give better
	* sound quality.
	*/

	md_mode |= (DMODE_SOFT_MUSIC | DMODE_16BITS);
	md_mixfreq = 0;
	md_reverb = 1;

	MikMod_Init(""), MikMod_strerror(MikMod_errno), 0;

	return replayerData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodDestroy(void* userData)
{
    MikMod_Exit();
    md_mixfreq = 0;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodOpen(void* userData, const char* buffer)
{
    MODULE* module;
	struct mikmodReplayerData* replayerData = (struct MikmodReplayerData*)userData;	

	// TODO: Use IO-api 
	module = Player_Load(buffer, 64, 0);

	if (!module)
		return 0;

    module->extspd = 1;
    module->panflag = 1;
    module->wrap = 0;
    module->loop = 0;

    if (md_mixfreq == 0)
        md_mixfreq = (!sample->desired.rate) ? 44100 : sample->desired.rate;

    Player_Start(module);
    Player_SetPosition(0);

    sample->flags = SOUND_SAMPLEFLAG_NONE;

    internal->total_time = (module->sngtime * 1000) / (1<<10);

    printf("MIKMOD: Name: %s\n", module->songname);
    printf("MIKMOD: Type: %s\n", module->modtype);

    Player_SetPosition(0);
    MikMod_Update();

    return(1); 

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodReadData(void* userData, void* dest)
{
	int8_t* newDest = (int8_t*)dest;

	// TODO: Support more than one tune

	struct mikmodReplayerData* replayerData = (struct mikmodReplayerData*)userData;	
	int decodeSize = hvl_DecodeFrame(replayerData->tune, newDest, &newDest[2], 4);
	//printf("decodeSize %d\n", decodeSize);

	/*
	if (decodeSize != size)
	{
		printf("missmatching decode size %d %d\n", decodeSize, size);
	}
	*/

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodSeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodFrameSize(void* userData)
{
	return 44100 * sizeof(uint16_t) * 2 / 50;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_mikmodPlugin = 
{
	1,
	mikmodInfo,
	mikmodTrackInfo,
	mikmodSupportedExtensions,
	mikmodCreate,
	mikmodDestroy,
	mikmodOpen,
	mikmodClose,
	mikmodReadData,
	mikmodSeek,
	mikmodFrameSize,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_mikmodPlugin;
}



