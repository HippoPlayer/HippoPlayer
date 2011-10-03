
#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mikmod.h"
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct MikmodReplayerData
{
	MODULE* module;
} MikmodReplayerData;

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
	//MikMod_RegisterDriver(&drv_raw);
	//MikMod_RegisterDriver(&drv_osx);
	MikMod_RegisterAllLoaders();

	md_mode |= (DMODE_SOFT_MUSIC | DMODE_STEREO | DMODE_16BITS | DMODE_HQMIXER);
	md_mixfreq = 44100;
	md_reverb = 1;

	MikMod_Init(""); //, MikMod_strerror(MikMod_errno), 0;

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
	MikmodReplayerData* replayerData = (MikmodReplayerData*)userData;	

	// TODO: Use IO-api 
	module = Player_Load((char*)buffer, 64, 0);

	if (!module)
		return 0;

    module->extspd = 1;
    module->panflag = 1;
    module->wrap = 0;
    module->loop = 0;

    //if (md_mixfreq == 0)
    md_mixfreq = 44100;

    Player_SetPosition(0);
    Player_Start(module);

    //internal->total_time = (module->sngtime * 1000) / (1<<10);

    printf("MIKMOD: Name: %s\n", module->songname);
    printf("MIKMOD: Type: %s\n", module->modtype);

    replayerData->module = module;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mikmodReadData(void* userData, HippoPlaybackBuffer* dest)
{
	MikmodReplayerData* replayer = (MikmodReplayerData*)userData;

	if (!Player_Active())
		Player_Start(replayer->module);

    int size = VC_WriteBytes((SBYTE*)dest->data, dest->frameSize);

    printf("%d : %d\n", replayer->module->sngpos, replayer->module->patpos);

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
	return 16 * 1024;
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
	0,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


HippoPlaybackPlugin* getPlugin()
{
	return &g_mikmodPlugin;
}



