#include <HippoPlugin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gbs.h>
#include <gbhw.h>

#define FREQ 48000

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: TS

static int16_t samples[1024];
static struct gbhw_buffer s_buffer = {
	.data    = samples,
	.bytes   = sizeof(samples),
	.samples = sizeof(samples) / 4,
	.pos     = 0,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	struct gbs* song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* gbsplay_supported_extensions() {
	return "gbs";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* gbsplay_create(const HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct ReplayerData));
	memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_destroy(void* user_data) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;

	if (data->song) {
		gbs_close(data->song);
	}

	free(data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct gbs* open_gbs(const char* filename, const struct HippoIoAPI* io) {
    uint64_t size = 0;
    void* data;

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to open: %s", filename);
        return NULL;
    }

    struct gbs* song = gbs_open_mem(filename, (char*)data, size);

    HippoIo_free_file_to_memory(io, data);

	return song;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_open(void* user_data, const char* filename, int subsong) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;

	if (!(data->song = open_gbs(filename, g_io_api))) {
        hp_error("Unable to open: %s", filename);
		return -1;
	}

    gbs_init(data->song, subsong);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_close(void* user_data) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;
	gbs_close(data->song);
	data->song = NULL;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult gbsplay_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	struct gbs* song;
	if ((song = gbs_open_mem(filename, (char*)data, data_size))) {
		gbs_close(song);
        hp_info("Supported: %s", filename);
	    return HippoProbeResult_Supported;
	}

	hp_debug("Unsupported: %s", filename);
	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct CallbackData {
	void* dest;
	int max_count;
	int ret_count;
} CallbackData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void callback(struct gbhw_buffer* buf, void* priv) {
	CallbackData* data = (CallbackData*)priv;
	int count = buf->bytes / 2;

	float* output = (float*)data->dest;
	int16_t* temp_data = (int16_t*)buf->data;

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < count; ++i) {
		const float v = ((float)temp_data[i]) * scale;
		*output++ = v;
	}

	data->ret_count = count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
	struct ReplayerData* data = (struct ReplayerData*)user_data;
	int workunit = (1000 * s_buffer.samples) / FREQ;
	CallbackData cb_data = { dest, samples_to_read, 0 };
	gbhw_setcallback(callback, &cb_data);
	gbs_step(data->song, workunit);
	return cb_data.ret_count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gbsplay_metadata(const char* filename, const HippoServiceAPI* service_api) {
    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    struct gbs* song = open_gbs(filename, io_api);

    if (!song) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

	hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, song->title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, song->author);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "Game Boy Sound System");
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, song->copyright);
	HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, 0.0f);

	for (int i = 0; i < song->songs; ++i) {
		const struct gbs_subsong_info* info = &song->subsong_info[i];
		printf("%d - %d %s\n", i, info->len, info->title);
	}

    if (song->songs > 1) {
        for (int i = 0; i < song->songs; ++i) {
        	const struct gbs_subsong_info* info = &song->subsong_info[i];
            char subsong_name[1024] = {0};

            if (info->title != NULL) {
                sprintf(subsong_name, "%s - %s (%d/%d)", song->title, info->title, i + 1, song->songs);
            } else {
                sprintf(subsong_name, "%s (%d/%d)", song->title, i + 1, song->songs);
            }

            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, info->len / 1024.0f);
        }
    }

    gbs_close(song);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void gbsplay_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void gbsplay_set_log(struct HippoLogAPI* log) {
	gbhw_setrate(FREQ);
	gbhw_setbuffer(&s_buffer);
	gbhw_setcallback(callback, NULL);
	g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_gbsplay_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"gbsplay",
	"0.0.1",
	"gbsplay git:549023c",
	gbsplay_probe_can_play,
	gbsplay_supported_extensions,
	gbsplay_create,
	gbsplay_destroy,
	gbsplay_event,
	gbsplay_open,
	gbsplay_close,
	gbsplay_read_data,
	gbsplay_seek,
	gbsplay_metadata,
    gbsplay_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_gbsplay_plugin;
}

