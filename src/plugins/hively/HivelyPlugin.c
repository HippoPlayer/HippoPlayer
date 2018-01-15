#include "../../plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <math.h>
#include "replayer/hvl_replay.h"

#define FREQ 48000
#define FRAME_SIZE ((FREQ * 2) / 50)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HivelyReplayerData {
	struct hvl_tune* tune;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hively_info(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hively_track_info(void* userData) {
	struct HivelyReplayerData* user_data = (struct HivelyReplayerData*)userData;
	return user_data->tune->ht_Name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hively_supported_extensions() {
	return "ahx,hvl";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* hively_create() {
	void* data = malloc(sizeof(struct HivelyReplayerData));
	memset(data, 0, sizeof(struct HivelyReplayerData));

	hvl_InitReplayer();

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_destroy(void* user_data) {
	struct HivelyReplayerData* data = (struct HivelyReplayerData*)user_data;
	free(data);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_open(void* userData, const char* buffer) {
	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc

	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)userData;
	replayerData->tune = hvl_load_tune(buffer, FREQ, 0);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_close(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_read_data(void* userData, void* dest) {
	int16_t temp_data[FRAME_SIZE * 4];
	int8_t* ptr = (int8_t*)temp_data;

	float* newDest = (float*)dest;

	// TODO: Support more than one tune
	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)userData;

	int frames_decoded = hvl_DecodeFrame(replayerData->tune, ptr, ptr + 2, 4) / 2;

	const float scale = 1.0f / 32768.0f;

	for (int i = 0; i < frames_decoded; ++i) {
		newDest[i] = ((float)temp_data[i]) * scale;
	}

	return frames_decoded;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_seek(void* userData, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_length(void* userData) {
	return -10;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_frame_size(void* userData) {
	return FRAME_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hively_plugin = {
	1,
	hively_info,
	hively_track_info,
	hively_supported_extensions,
	hively_create,
	hively_destroy,
	hively_open,
	hively_close,
	hively_read_data,
	hively_seek,
	hively_frame_size,
	hively_length,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* getPlugin() {
	return &g_hively_plugin;
}


