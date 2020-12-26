#include <HippoPlugin.h>
#include <wavpack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
    WavpackContext* song;
    uint32_t sample_rate;
    uint32_t bytes_per_sample;
    uint8_t channel_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* wavpack_supported_extensions() {
    return "wav,mv";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* wavpack_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int wavpack_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if (data->song) {
        WavpackCloseFile(data->song);
    }

    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int wavpack_open(void* user_data, const char* filename, int subsong) {
    char error_buff[256];
    struct ReplayerData* data = (struct ReplayerData*)user_data;

	data->song = WavpackOpenFileInput(filename, error_buff, OPEN_TAGS | OPEN_WVC | OPEN_NORMALIZE, 0);

    if (!data->song) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    data->sample_rate = WavpackGetSampleRate(data->song);
    data->channel_count = WavpackGetNumChannels(data->song);
	data->bytes_per_sample = WavpackGetBytesPerSample(data->song);

	hp_debug("Open wavpack file with sample rate %d channels %d bytes per sample %d\n",
		data->sample_rate, data->channel_count, data->bytes_per_sample);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int wavpack_close(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
	WavpackCloseFile(data->song);
    data->song = NULL;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult wavpack_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    char error_buff[256];
	// TODO: Implement proper reader here
	WavpackContext* ctx = WavpackOpenFileInput(filename, error_buff, OPEN_TAGS | OPEN_WVC | OPEN_NORMALIZE, 0);

    if (ctx) {
		WavpackCloseFile(ctx);
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo wavpack_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

    WavpackUnpackSamples(data->song, (int32_t*)dest, samples_to_read);

	uint8_t output_mode = HippoOutputType_s16;

	if (WavpackGetMode(data->song) & MODE_FLOAT) {
		output_mode = HippoOutputType_f32;
	}

    HippoReadInfo t = {
        data->sample_rate,
        samples_to_read * 2,
        data->channel_count,
        output_mode
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int wavpack_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct IdMap {
	const char* wavpack_id;
	const char* metadata_id;
} IdMap;

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int wavpack_metadata(const char* filename, const HippoServiceAPI* service_api) {
    char text_buffer[1024];
    char error_buff[256];

    //const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

	// TODO: Use reading APIs
	WavpackContext* ctx = WavpackOpenFileInput(filename, error_buff, OPEN_TAGS | OPEN_WVC | OPEN_NORMALIZE, 0);

    if (!ctx) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

    hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    static IdMap s_ids[] = {
    	 { "Title", HippoMetadata_TitleTag },
    	 { "Album", HippoMetadata_AlbumTag },
    	 { "Artist", HippoMetadata_ArtistTag },
    	 { "Comment", HippoMetadata_MessageTag },
    	 { "Genre", HippoMetadata_GenreTag },
    	 { "Year", HippoMetadata_DateTag },
    };

    for (int i = 0; i < sizeof_array(s_ids); ++i) {
		if (WavpackGetTagItem(ctx, s_ids[i].wavpack_id, text_buffer, sizeof(text_buffer))) {
			HippoMetadata_set_tag(metadata_api, index, s_ids[i].metadata_id, text_buffer);
		}
    }

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, WavpackGetFileExtension(ctx));

	HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag,
		WavpackGetNumSamples(ctx) / (float)WavpackGetSampleRate(ctx));

	WavpackCloseFile(ctx);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void wavpack_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void wavpack_static_init(struct HippoLogAPI* log, const HippoServiceAPI* service_api) {
	(void)service_api;
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin s_wavpack_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "wavpack",
    "0.0.1",
    "wavpack 5.3.0",
    wavpack_probe_can_play,
    wavpack_supported_extensions,
    wavpack_create,
    wavpack_destroy,
    wavpack_event,
    wavpack_open,
    wavpack_close,
    wavpack_read_data,
    wavpack_seek,
    wavpack_metadata,
    wavpack_static_init,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &s_wavpack_plugin;
}

