#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <StSoundLibrary.h>

#define FREQ 48000
#define FRAME_SIZE 4096

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	YMMUSIC* song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* stsound_supported_extensions() {
	return "ym";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* stsound_create(const HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct ReplayerData));
	memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_destroy(void* user_data) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;

	if (data->song) {
        ymMusicDestroy(data->song);
	}

	free(data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_open(void* user_data, const char* filename, int subsong) {
    uint64_t size = 0;
    void* file_data;
	struct ReplayerData* data = (struct ReplayerData*)user_data;

    HippoIoErrorCode res = HippoIo_read_file_to_memory(g_io_api, filename, &file_data, &size);

    if (res < 0) {
        return -1;
    }

    // we don't need to keep the data around after reading in the song
    HippoIo_free_file_to_memory(g_io_api, file_data);

    data->song = ymMusicCreate();

    if (!ymMusicLoadMemory(data->song, file_data, size)) {
        ymMusicDestroy(data->song);
        hp_error("Unable to open: %s", filename);
        data->song = nullptr;
        return -1;
    }

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_close(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult stsound_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    YMMUSIC* song = ymMusicCreate();
    HippoProbeResult res;

    if (ymMusicLoadMemory(song, (void*)data, data_size)) {
        hp_info("Supported: %s", filename);
	    res = HippoProbeResult_Supported;
    } else {
        hp_debug("Unsupported: %s", filename);
	    res = HippoProbeResult_Unsupported;
    }

    ymMusicDestroy(song);

    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
	ReplayerData* data = (ReplayerData*)user_data;
	float* output = (float*)dest;

	int16_t temp_data[FRAME_SIZE * 2] = { 0 };
    samples_to_read = hippo_min(samples_to_read, FRAME_SIZE);

    ymMusicCompute(data->song, temp_data, samples_to_read);

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < samples_to_read; ++i) {
		const float v = ((float)temp_data[i]) * scale;
		*output++ = v;
		*output++ = v;
	}

	return samples_to_read * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int stsound_metadata(const char* filename, const HippoServiceAPI* service_api) {
    void* data = 0;
    uint64_t size = 0;

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    HippoIo_free_file_to_memory(io_api, data);

    YMMUSIC* song = ymMusicCreate();

    if (!ymMusicLoadMemory(song, data, size)) {
        ymMusicDestroy(song);
        hp_error("Unable to init %s", filename);
        return -1;
    }

	hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    ymMusicInfo_t music_info;
    ymMusicGetInfo(song, &music_info);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, music_info.pSongName);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, music_info.pSongAuthor);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, music_info.pSongType);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, music_info.pSongComment);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, music_info.musicTimeInMs / 1000.0f);

    ymMusicDestroy(song);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void stsound_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void stsound_set_log(struct HippoLogAPI* log) { g_hp_log = log; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_stsound_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"stsound",
	"0.0.1",
	"StSoundLibrary 1.43",
	stsound_probe_can_play,
	stsound_supported_extensions,
	stsound_create,
	stsound_destroy,
	stsound_event,
	stsound_open,
	stsound_close,
	stsound_read_data,
	stsound_seek,
	stsound_metadata,
    stsound_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_stsound_plugin;
}



