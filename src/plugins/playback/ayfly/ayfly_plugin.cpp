#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ayfly.h>
#include <math.h>

#define FREQ 48000
#define FRAME_SIZE 4096

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	void* song;
	int read_index;
	int frames_decoded;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* ayfly_supported_extensions() {
	return "ay,vtx,ym,psg,asc,pt1,pt2,pt3,stc,stp,psc,pqt";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* ayfly_create(const HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct ReplayerData));
	memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_destroy(void* user_data) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;

	if (data->song) {
        ay_closesong(&data->song);
	}

	free(data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_open(void* user_data, const char* filename, int subsong) {
    uint64_t size = 0;
    void* file_data;
	struct ReplayerData* data = (struct ReplayerData*)user_data;

    HippoIoErrorCode res = HippoIo_read_file_to_memory(g_io_api, filename, &file_data, &size);

    if (res < 0) {
        return -1;
    }

    data->song = ay_initsongindirect((unsigned char*)file_data, FREQ, size);
    ay_setoversample(data->song, 1);

    // we don't need to keep the data around after reading in the song
    HippoIo_free_file_to_memory(g_io_api, file_data);

    if (!data->song) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_close(void* user_data) {

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult ayfly_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    if (ay_can_play((void*)data, (int)data_size)) {
        hp_info("Supported: %s", filename);
	    return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
	int16_t temp_data[FRAME_SIZE * 2];
	uint8_t* ptr = (uint8_t*)temp_data;

	float* newDest = (float*)dest;
	samples_to_read = hippo_min(FRAME_SIZE, samples_to_read);

	// TODO: Support more than one tune
	struct ReplayerData* replayer_data = (struct ReplayerData*)user_data;

    int written = (int)ay_rendersongbuffer(replayer_data->song, ptr, samples_to_read);

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < written; ++i) {
		newDest[i] = ((float)temp_data[i]) * scale;
	}

	return written / 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int ayfly_metadata(const char* filename, const HippoServiceAPI* service_api) {
    void* data = 0;
    uint64_t size = 0;

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    AYSongInfo* song = (AYSongInfo*)ay_initsongindirect((uint8_t*)data, FREQ, size);

    HippoIo_free_file_to_memory(io_api, data);

    if (!song) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

	hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, song->Name.c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "ayfly (temp)");
    if (song->PrgName.length() != 0) {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AuthoringToolTag, song->PrgName.c_str());
    }

    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, song->Length / 50);

    ay_closesong((void**)&song);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ayfly_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ayfly_set_log(struct HippoLogAPI* log) { g_hp_log = log; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_ayfly_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"ayfly",
	"0.0.1",
	"ayfly 0.0.25",
	ayfly_probe_can_play,
	ayfly_supported_extensions,
	ayfly_create,
	ayfly_destroy,
	ayfly_event,
	ayfly_open,
	ayfly_close,
	ayfly_read_data,
	ayfly_seek,
	ayfly_metadata,
    ayfly_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_ayfly_plugin;
}



