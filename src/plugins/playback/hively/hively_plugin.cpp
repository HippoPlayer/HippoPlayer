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
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HivelyReplayerData {
	struct hvl_tune* tune;
	void* song_data;
	int16_t temp_data[FRAME_SIZE * 4];
	int read_index;
	int frames_decoded;
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

static int hively_open(void* user_data, const char* filename, int subsong) {
    uint64_t size = 0;
	struct HivelyReplayerData* data = (struct HivelyReplayerData*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data, filename, &data->song_data, &size);

    if (res < 0) {
        return -1;
    }

    hp_info("Starting to play %s (subsong %d)", filename, subsong);

	data->tune = hvl_LoadTuneMemory((uint8_t*) data->song_data, (int)size, FREQ, 0);
    hvl_InitSubsong(data->tune, subsong);

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
		hp_info("Supported: %s", filename);
		return HippoProbeResult_Supported;
	}

	if ((data[0] == 'H') &&
		(data[1] == 'V') &&
		(data[2] == 'L')) {
		hp_info("Supported: %s", filename);
		return HippoProbeResult_Supported;
	}

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int hively_read_data(void* user_data, void* dest, uint32_t samples_to_read_in) {
	struct HivelyReplayerData* data = (struct HivelyReplayerData*)user_data;
	int8_t* ptr = (int8_t*)data->temp_data;
	float* newDest = (float*)dest;
	int samples_to_read = (int)samples_to_read_in;

	// because hively works with fixed sized chunks we need to do some temporary stuff here as
	// the player can request arbitary sized chunk. Later on it would be good to allow the backend
	// to just deal with this insdeat

	if (data->frames_decoded == 0) {
	    data->frames_decoded = hvl_DecodeFrame(data->tune, ptr, ptr + 2, 4) / 4;
	}

	const float scale = 1.0f / 32767.0f;

	int16_t* data_read = (int16_t*)data->temp_data;

	// if we have enough data we can just convert it to the output
	if ((data->read_index + samples_to_read) < data->frames_decoded) {
	    data_read += data->read_index * 2;

        for (int i = 0; i < samples_to_read * 2; ++i) {
            newDest[i] = ((float)data_read[i]) * scale;
        }

        data->read_index += samples_to_read;
	} else {
	    // else we need to copy what we have left, decode a new frame and copy the remainder from thata
	    int diff = data->frames_decoded - data->read_index;
	    data_read += data->read_index * 2;

	    // copy what we have left in the buffer
        for (int i = 0; i < diff * 2; ++i) {
            newDest[i] = ((float)data_read[i]) * scale;
        }

        newDest += diff * 2;

        // decode some new output so we can fill up the remining data
	    data->frames_decoded = hvl_DecodeFrame(data->tune, ptr, ptr + 2, 4) / 4;
	    data_read = (int16_t*)data->temp_data;
	    int new_samples = samples_to_read - diff;

	    // copy what we have left in the buffer
        for (int i = 0; i < new_samples * 2; ++i) {
            newDest[i] = ((float)data_read[i]) * scale;
        }

        data->read_index = new_samples;
	}

	//int frames_decoded = hvl_DecodeFrame(replayerData->tune, ptr, ptr + 2, 4) / 2;

	return 0;
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
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    bool is_ahx = true;
    uint8_t* t = (uint8_t*)data;

    if((t[0] == 'H') && (t[1] == 'V') && (t[2] == 'L')) {
        is_ahx = false;
    }

	struct hvl_tune* tune = hvl_LoadTuneMemory((uint8_t*) data, (int)size, FREQ, 0);

	hp_info("Updating metadata for %s", filename);

    // TODO: Calculate len
    float length = 0.0f;

    const char* tool = is_ahx ? "AHX Tracker" : "Hively Tracker";

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, tune->ht_Name);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, tool);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AuthoringToolTag, tool);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, length);

    // instruments starts from 1 in hively so skip 0
    for (int i = 1; i < tune->ht_InstrumentNr; ++i) {
        HippoMetadata_add_instrument(metadata_api, index, tune->ht_Instruments[i].ins_Name);
	}

	if (tune->ht_SubsongNr > 1) {
	    for (int i = 0, c = tune->ht_SubsongNr; i < c; ++i) {
            char subsong_name[1024] = { 0 };
            sprintf(subsong_name, "%s (%d/%d)", tune->ht_Name, i + 1, tune->ht_SubsongNr);
            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
	    }
	}

    // Make sure to free the buffer before we leave
    HippoIo_free_file_to_memory(io_api, data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void hively_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void hively_static_init(struct HippoLogAPI* log, const HippoServiceAPI* service_api) {
    (void)service_api;
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_hively_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"Hively",
	"0.0.1",
	"",
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
    hively_static_init,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_hively_plugin;
}


