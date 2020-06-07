#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
extern "C" {
#include "replayer/hvl_replay.h"
}

#define FREQ 48000
#define FRAME_SIZE ((FREQ * 2) / 50)

static const struct HippoIoAPI* g_io_api = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HivelyReplayerData {
	struct hvl_tune* tune;
	void* song_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* hively_supported_extensions() {
	return "ahx,hvl";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* hively_create(const HippoServiceAPI* service_api) {
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

static int hively_close(void* user_data) {
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
		(data[2] == 'L')) {
		return HippoProbeResult_Supported;
	}

    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_read_data(void* user_data, void* dest, uint32_t max_count) {
	int16_t temp_data[FRAME_SIZE * 4];
	int8_t* ptr = (int8_t*)temp_data;

	float* newDest = (float*)dest;

	// TODO: Support more than one tune
	struct HivelyReplayerData* replayerData = (struct HivelyReplayerData*)user_data;

	int frames_decoded = hvl_DecodeFrame(replayerData->tune, ptr, ptr + 2, 4) / 2;

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < frames_decoded; ++i) {
		newDest[i] = ((float)temp_data[i]) * scale;
	}

	return frames_decoded;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_metadata(const char* filename, const HippoServiceAPI* service_api) {
    void* data = 0;
    uint64_t size = 0;

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, &data, &size);

    if (res < 0) {
        return -1;
    }

    bool is_ahx = true;
    uint8_t* t = (uint8_t*)data;

    if((t[0] == 'H') && (t[1] == 'V') && (t[2] == 'L')) {
        is_ahx = false;
    }

	struct hvl_tune* tune = hvl_LoadTuneMemory((uint8_t*) data, (int)size, FREQ, 0);

    // TODO: Calculate len
    float length = 0.0f;

    std::vector<flatbuffers::Offset<flatbuffers::String>> instruments;
    std::vector<flatbuffers::Offset<flatbuffers::String>> samples;

    flatbuffers::FlatBufferBuilder builder(4096);

    const char* tool = is_ahx ? "AHX Tracker" : "Hively Tracker";

    auto url = builder.CreateString(filename);
    auto title = builder.CreateString(tune->ht_Name);
    auto song_type = builder.CreateString(tool);
    auto authoring_tool = builder.CreateString(tool);
    auto artist = builder.CreateString("");
    auto date = builder.CreateString("");
    auto message = builder.CreateString("");

    for (int i = 0; i < tune->ht_InstrumentNr; ++i) {
	    instruments.push_back(builder.CreateString(tune->ht_Instruments[i].ins_Name));
    }

    builder.Finish(CreateHippoMessageDirect(builder, MessageType_song_metadata,
        CreateHippoSongMetadata(builder,
            url,
            title,
            song_type,
            length,
            authoring_tool,
            artist,
            date,
            message,
            builder.CreateVector(samples),
            builder.CreateVector(instruments)).Union()));

    HippoMetadata_set_data(metadata_api, filename, builder.GetBufferPointer(), builder.GetSize());

    // Make sure to free the buffer before we leave
    HippoIo_free_file_to_memory(io_api, data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int hively_length(void* user_data) {
	return -10;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void hively_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hively_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"Hively",
	"0.0.1",
	hively_probe_can_play,
	hively_supported_extensions,
	hively_create,
	hively_destroy,
	hively_event,
	hively_open,
	hively_close,
	hively_read_data,
	hively_seek,
	hively_metadata,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_hively_plugin;
}


