#include <HippoPlugin.h>
#include <taglib_metadata.h>
#include <stdint.h>
#include "minivorbis.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	OggVorbis_File song;
	uint32_t sample_rate;
	uint8_t channel_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* oggvorbis_supported_extensions() {
    return "ogg";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* oggvorbis_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int oggvorbis_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

	ov_clear(&data->song);
    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int oggvorbis_open(void* user_data, const char* filename, int subsong) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

	// TODO: Use IO APIs
    if (ov_fopen(filename, &data->song) < 0) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

	vorbis_info* info = ov_info(&data->song, -1);
    printf("Ogg file %d Hz, %d channels, %d kbit/s.\n", (int)info->rate, (int)info->channels, (int)info->bitrate_nominal / 1024);

    data->sample_rate = info->rate;
    data->channel_count = info->channels;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int oggvorbis_close(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
	ov_clear(&data->song);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult oggvorbis_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    OggVorbis_File song;

	// TODO: Implement proper reader here
    if (ov_fopen(filename, &song) >= 0) {
		ov_clear(&song);
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo oggvorbis_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

    int section = 0;
	int ret = ov_read(&data->song, dest, samples_to_read, 0, 2, 1, &section);

    HippoReadInfo t = {
    	data->sample_rate,
        ret / 4,
        data->channel_count,
        HippoOutputType_s16,
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int oggvorbis_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int oggvorbis_metadata(const char* filename, const HippoServiceAPI* service_api) {
    //const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoMetadataId index = taglib_update_metadata(filename, metadata_api);

    if (index == (uint64_t)~0) {
        hp_info("Unable to init metadata for %s", filename);
    	return -1;
    }

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "Ogg Vorbis");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void oggvorbis_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void oggvorbis_set_log(struct HippoLogAPI* log) {
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin s_oggvorbis_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "oggvorbis",
    "0.0.1",
    "dr_oggvorbis 0.6.24",
    oggvorbis_probe_can_play,
    oggvorbis_supported_extensions,
    oggvorbis_create,
    oggvorbis_destroy,
    oggvorbis_event,
    oggvorbis_open,
    oggvorbis_close,
    oggvorbis_read_data,
    oggvorbis_seek,
    oggvorbis_metadata,
    oggvorbis_set_log,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &s_oggvorbis_plugin;
}




