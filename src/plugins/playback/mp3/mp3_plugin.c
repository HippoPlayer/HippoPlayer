#include <HippoPlugin.h>
#include <taglib_metadata.h>
#include "dr_mp3.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	drmp3 song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* mp3_supported_extensions() {
    return "mp3";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mp3_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mp3_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

	drmp3_uninit(&data->song);
    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mp3_open(void* user_data, const char* filename, int subsong, const struct HippoSettingsAPI* api) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

	// TODO: Use IO APIs
    if (!drmp3_init_file(&data->song, filename, NULL)) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mp3_close(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
	drmp3_free(&data->song, NULL);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult mp3_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    drmp3 song;

	// TODO: Implement proper reader here
    if (drmp3_init_file(&song, filename, NULL)) {
    	drmp3_uninit(&song);
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo mp3_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 8, FRAME_SIZE);

	drmp3_read_pcm_frames_f32(&data->song, samples_to_read, (float*)dest);

    HippoReadInfo t = {
    	data->song.sampleRate,
        samples_to_read,
        data->song.channels,
        HippoOutputType_f32,
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mp3_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mp3_metadata(const char* filename, const HippoServiceAPI* service_api) {
    //const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoMetadataId index = taglib_update_metadata(filename, metadata_api);

    printf("index %ld\n", index);
    printf("updating metadata\n");

    if (index == (uint64_t)~0) {
        hp_info("Unable to init metadata for %s", filename);
    	return -1;
    }

    printf("updating songtype\n");
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "MP3");
    printf("end songtype\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mp3_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mp3_static_init(struct HippoLogAPI* log, const HippoServiceAPI* service_api) {
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin s_mp3_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "mp3",
    "0.0.1",
    "dr_mp3 0.6.24",
    mp3_probe_can_play,
    mp3_supported_extensions,
    mp3_create,
    mp3_destroy,
    mp3_event,
    mp3_open,
    mp3_close,
    mp3_read_data,
    mp3_seek,
    mp3_metadata,
    mp3_static_init,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &s_mp3_plugin;
}



