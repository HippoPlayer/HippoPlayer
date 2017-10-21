#include "../../plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/SidInfo.h>
#include <builders/residfp-builder/residfp.h>

const int FREQ = 48000;
const int FRAME_SIZE = (FREQ * 2) / 100;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SidReplayerData {
    sidplayfp engine;
	ReSIDfpBuilder* rs = nullptr;
	SidTune* tune = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sid_info(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sid_track_info(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sid_supported_extensions() {
	return "sid";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* sid_create() {
	SidReplayerData* data = new SidReplayerData();

    data->engine.setRoms(nullptr, nullptr, nullptr);

	data->rs = new ReSIDfpBuilder("HippoPlayer");
    // Get the number of SIDs supported by the engine
    unsigned int max_sids = data->engine.info().maxsids();

	// Create SID emulators
    data->rs->create(max_sids);
    
    // Check if builder is ok
    if (!data->rs->getStatus())
    {
        printf("SidPlugin error %s\n", data->rs->error());
        return 0;
    }

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_destroy(void* user_data) {
	struct SidReplayerData* t = (struct SidReplayerData*)user_data;
	t->engine.stop();
	//delete t;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_open(void* user_data, const char* buffer) {
	SidReplayerData* data = (SidReplayerData*)user_data;
	
	SidTune* tune = new SidTune(buffer);

    // CHeck if the tune is valid
    if (!tune->getStatus())
    {
        printf("SidPlugin: tune status %s\n", tune->statusString());
        return -1;
    }

    data->tune = tune;

    // Select default song
    tune->selectSong(0);

    // Configure the engine
    SidConfig cfg;
    cfg.frequency = FREQ;
    cfg.samplingMethod = SidConfig::INTERPOLATE;
    cfg.fastSampling = false;
    cfg.playback = SidConfig::MONO;
    cfg.sidEmulation = data->rs;

    if (!data->engine.config(cfg))
    {
        printf("Engine error %s\n", data->engine.error());
        return 0;
    }

    // Load tune into engine
    if (!data->engine.load(data->tune))
    {
        printf("Engine error %s\n", data->engine.error());
        return 0;
    }

    printf("sid_open ok\n");

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_close(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_read_data(void* user_data, void* dest) {
	SidReplayerData* data = (SidReplayerData*)user_data;
	float* output = (float*)dest;

	int16_t temp_data[FRAME_SIZE * 2];

	data->engine.play(temp_data, FRAME_SIZE / 2);

	const float scale = 1.0f / 32768.0f;

	/*
	static float s_count = 0.0f;
	s_count += 0.02;

	for (int i = 0; i < FRAME_SIZE; ++i) {
		//newDest[i] = ((float)temp_data[i]) * scale;
		output[i] = sin(s_count); 
	}
	*/

	for (int i = 0; i < FRAME_SIZE / 2; ++i) {
		const float v = ((float)temp_data[i]) * scale;
		*output++ = v;
		*output++ = v;
	}

	return FRAME_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_seek(void* userData, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_frame_size(void* userData) {
	return FRAME_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_sid_plugin = {
	1,
	sid_info,
	sid_track_info,
	sid_supported_extensions,
	sid_create,
	sid_destroy,
	sid_open,
	sid_close,
	sid_read_data,
	sid_seek,
	sid_frame_size,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HippoPlaybackPlugin* getPlugin() {
	return &g_sid_plugin;
}


