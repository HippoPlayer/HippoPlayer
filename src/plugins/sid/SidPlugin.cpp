
#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define HAVE_UNIX
#include <sidplay2.h>
#include <builders/resid.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SidReplayerData
{
	SidTune* tune;
	sidplay2* player;
	ReSIDBuilder* builder;
	sid_filter_t filterSettings;
	char tuneData[70 * 1024];	// some extra bytes for good messure
	int tuneSize;
};

static struct SidReplayerData g_replayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sidInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sidTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** sidSupportedExtensions(void* userData)
{
	static const char* supportedFomats[] =
	{
		"sid",
		"psid",
		0,
	};

	return supportedFomats;
}

/*
 *	mPlaybackSettings.mFilterKinkiness = 0.17;
	mPlaybackSettings.mFilterBaseLevel = 210.0f;
	mPlaybackSettings.mFilterOffset = -375.0f;
	mPlaybackSettings.mFilterSteepness = 120.0f;
	mPlaybackSettings.mFilterRolloff = 5.5f;
	mPlaybackSettings.mDistortionRate = 1500;
	mPlaybackSettings.mDistortionHeadroom = 400;
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static float approximate_dac(int x, float kinkiness)
{
    float bits = 0.0f;
    for (int i = 0; i < 11; i += 1)
        if (x & (1 << i))
            bits += pow(i, 4) / pow(10, 4);

    return x * (1.0f + bits * kinkiness);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void setFilterSettingsFromPlaybackSettings(sid_filter_t& filterSettings)
{
	//filterSettings.distortion_enable = true;
	//filterSettings.rate = 1500;
	//filterSettings.headroom = 400;

	if (false /*settings->mFilterType == SID_FILTER_8580*/)
	{
		//filterSettings.opmin = -99999;
		//filterSettings.opmax = 99999;

		//filterSettings.points = sDefault8580PointCount;
		//memcpy(filterSettings.cutoff, sDefault8580, sizeof(sDefault8580));
	}
	else
	{
		//filterSettings.opmin = -20000;
		//filterSettings.opmax = 20000;

		filterSettings.points = 0x800;

		for (int i = 0; i < 0x800; i++) 
		{
			float i_kinked = approximate_dac(i, 0.17);
			float freq = 210.0f + powf(2.0f, (i_kinked - -375.0f) / 120.0f);

			// Better expression for this required.
			// As it stands, it's kinda embarrassing.
			for (float j = 1000.f; j < 18500.f; j += 500.f)
			{
				if (freq > j)
					freq -= (freq - j) / 5.5f;
			}
			if (freq > 18500)
				freq = 18500;

			filterSettings.cutoff[i][0] = i;
			filterSettings.cutoff[i][1] = freq;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* sidCreate()
{
	// TODO: supply custom allocator

	SidReplayerData* replayerData = (SidReplayerData*)malloc(sizeof(struct SidReplayerData));
	memset(replayerData, 0, sizeof(struct SidReplayerData));

	sidplay2* player = new sidplay2;
	ReSIDBuilder* builder = new ReSIDBuilder("resid");

	replayerData->player = player; 
	replayerData->builder = builder;

	sid2_config_t cfg = player->config();
	
	cfg.clockSpeed    = SID2_CLOCK_PAL;
	cfg.clockDefault  = SID2_CLOCK_PAL;
	cfg.clockForced   = true;
	
	cfg.environment   = sid2_envR;
	cfg.playback	  = sid2_mono;
	cfg.forceDualSids = false;
	cfg.emulateStereo = false;

	cfg.optimisation = SID2_DEFAULT_OPTIMISATION;
	
	cfg.sidDefault	  = SID2_MOS6581;
	cfg.sidEmulation  = builder;
	cfg.sidSamples	  = true;

	// setup resid
	if (builder->devices(true) == 0)
		builder->create(1);

	setFilterSettingsFromPlaybackSettings(replayerData->filterSettings);
		
	builder->filter(true);
	builder->filter(&replayerData->filterSettings);
	builder->sampling(cfg.frequency);
	
	int rc = player->config(cfg);
	if (rc != 0)
		printf("configure error: %s\n", player->error());

	//player->setRegisterFrameChangedCallback(NULL, NULL);

	return replayerData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidOpen(void* userData, const char* buffer)
{
	struct SidReplayerData* replayer = (struct SidReplayerData*)userData;	

	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc
	FILE* file = fopen(buffer, "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	fread(replayer->tuneData, size, 1, file);
	fclose(file);
	replayer->tuneSize = (int)size;

	replayer->tune = new SidTune((uint_least8_t *) replayer->tuneData, size);

    if (!replayer->tune)
        return false;

	replayer->tune->selectSong(0);

	int rc = replayer->player->load(replayer->tune);

	if (rc == -1)
	{
		delete replayer->tune;
		replayer->tune = 0;
		return -1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidClose(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidReadData(void* userData, HippoPlaybackBuffer* dest)
{
	int16_t buffer[8192];

	int16_t* newDest = (int16_t*)dest->data;
	struct SidReplayerData* replayerData = (struct SidReplayerData*)userData;	
	replayerData->player->play(buffer, dest->frameSize >> 1); // mono

	printf("filling buffer\n");

	// output is in stero so just copy the data in place
	// TODO: Proper handling of stereo/mono as player can be configed for stereo

	for (int i = 0; i < 4096; ++i)
	{
		short sample = buffer[i];
		newDest[0] = sample;
		newDest[1] = sample;
		newDest += 2;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidSeek(void* userData, int ms)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sidFrameSize(void* userData)
{
	return 4096 * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_sidPlugin = 
{
	1,
	sidInfo,
	sidTrackInfo,
	sidSupportedExtensions,
	sidCreate,
	sidDestroy,
	sidOpen,
	sidClose,
	sidReadData,
	sidSeek,
	sidFrameSize,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HippoPlaybackPlugin* getPlugin()
{
	return &g_sidPlugin;
}


