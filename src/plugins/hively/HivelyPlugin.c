#include "../../plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <math.h>
#include "replayer/hvl_replay.h"

#define FREQ 48000
#define FRAME_SIZE ((FREQ * 2) / 50)

static struct HippoIoAPI* g_io_api = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HivelyReplayerData {
	struct hvl_tune* tune;
	void* song_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static const char* hively_track_info(void* userData) {
	struct HivelyReplayerData* user_data = (struct HivelyReplayerData*)userData;
	return user_data->tune->ht_Name;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hively_supported_extensions() {
	return "ahx,hvl";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* hively_create(HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct HivelyReplayerData));
	memset(data, 0, sizeof(struct HivelyReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	hvl_InitReplayer();

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_destroy(void* user_data) {
	struct HivelyReplayerData* data = (struct HivelyReplayerData*)user_data;

	if (g_io_api) {
	    g_io_api->free_file_to_memory(g_io_api->priv_data, data->song_data);
	}

	free(data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_open(void* user_data, const char* filename) {
    uint64_t size = 0;
	struct HivelyReplayerData* data = (struct HivelyReplayerData*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data, filename, &data->song_data, &size);

    if (res < 0) {
        return -1;
    }

	data->tune = hvl_LoadTuneMemory((uint8_t*) data->song_data, (int)size, FREQ, 0);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_close(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult hively_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	if ((data[0] == 'T') &&
		(data[1] == 'H') &&
		(data[2] == 'X') &&
		(data[3] < 3)) {

		return HippoProbeResult_Supported;
	}

	if ((data[0] == 'H') &&
		(data[1] == 'V') &&
		(data[2] == 'L') &&
		(data[3] > 1)) {

		return HippoProbeResult_Supported;
	}

    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_read_data(void* userData, void* dest, uint32_t max_count) {
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

/*
static int hively_length(void* userData) {
	return -10;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hively_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	hively_probe_can_play,
	hively_supported_extensions,
	hively_create,
	hively_destroy,
	hively_open,
	hively_close,
	hively_read_data,
	hively_seek,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_hively_plugin;
}


