#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <stdlib.h>
#include <string.h>
#include "xgm/xgm.h"

#define SAMPLE_RATE 48000
#define CHANNELS 2

HippoLogAPI* g_hp_log = NULL;
static const struct HippoIoAPI* g_io_api = 0;
//static const HippoMetadataAPI* g_metadata_api = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a nsf plugin used as a reference and not inteded to do anything

typedef struct NsfPlugin {
    xgm::NSF nsf;
    xgm::NSFPlayer player;
    xgm::NSFPlayerConfig config;
} NsfPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* nsf_supported_extensions() {
	return "nsf,nsfe";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult nsf_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	if ((data[0] == 'N') && (data[1] == 'E') && (data[2] == 'S') && (data[3] == 'M')) {
	    hp_info("Supported: %s", filename);
		return HippoProbeResult_Supported;
	}

	if ((data[0] == 'N') && (data[1] == 'S') && (data[2] == 'F') && (data[3] == 'E')) {
	    hp_info("Supported: %s", filename);
	    return HippoProbeResult_Supported;
	}

    hp_debug("Unsupported: %s", filename);
	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_metadata(const char* filename, const HippoServiceAPI* service_api) {
    void* data = 0;
    uint64_t size = 0;

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to file-io open %s", filename);
        return res;
    }

    xgm::NSF nsf;

    // TODO: Error handling
    if (!nsf.Load((xgm::UINT8*)data, (int)size)) {
        hp_error("Unable to metadata open %s", filename);
        return -1;
    }

    hp_info("Updating metadata for %s", filename);

    // Validate this is correct
    //int length = nsf.time_in_ms < 0 ? nsf.default_playtime : nsf.time_in_ms;

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, nsf.title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "NES Sound Format");
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, nsf.artist);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, nsf.copyright);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, 0);

    // Make sure to free the buffer before we leave
    HippoIo_free_file_to_memory(io_api, data);

    int subsongs_count = nsf.songs;

    if (subsongs_count > 1) {
        for (int i = 0; i < subsongs_count; ++i) {
            char subsong_name[1024] = { 0 };
            sprintf(subsong_name, "%s (%d/%d)", nsf.title, i + 1, subsongs_count);
            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
        }
    }

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* nsf_create(const struct HippoServiceAPI* service_api) {
	NsfPlugin* plugin = new NsfPlugin;

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_open(void* user_data, const char* buffer, int subsong) {
    uint64_t size = 0;
    void* load_data = nullptr;
	NsfPlugin* data = (NsfPlugin*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data, buffer, &load_data, &size);

    if (res < 0) {
        hp_error("Unable to file-io open %s", buffer);
        return res;
    }

    if (!data->nsf.Load((xgm::UINT8*)load_data, (int)size)) {
        hp_error("Unable to load %s", buffer);
        return -1;
    }

    data->config["MASTER_VOLUME"] = 256; /* default volume = 128 */
    data->player.SetConfig(&data->config);

    if (!data->player.Load(&data->nsf)) {
        hp_error("Unable to open %s", buffer);
        return -1;
    }

    hp_info("Starting to play %s (subsong %d)", buffer, subsong);

    data->player.SetPlayFreq(SAMPLE_RATE);
    data->player.SetChannels(CHANNELS);
    data->player.SetSong(subsong);
    data->player.Reset();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_close(void* user_data) {
	NsfPlugin* plugin = (NsfPlugin*)user_data;
	(void)plugin;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_destroy(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
	NsfPlugin* plugin = (NsfPlugin*)user_data;
	(void)plugin;

    samples_to_read = 1024;

	int16_t data[1024 * 2] = { 0 };

	float* new_dest = (float*)dest;

	const float scale = 1.0f / 32767.0f;

	plugin->player.Render(data, samples_to_read);

	for (int i = 0; i < samples_to_read * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return samples_to_read * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int nsf_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void nsf_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)len;
    (void)data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void nsf_set_log(struct HippoLogAPI* log) { g_hp_log = log; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_nsf_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"nsf",
	"0.0.1",
    NSFPLAY_TITLE,
	nsf_probe_can_play,
	nsf_supported_extensions,
	nsf_create,
	nsf_destroy,
	nsf_event,
	nsf_open,
	nsf_close,
	nsf_read_data,
	nsf_plugin_seek,
	nsf_metadata,
	nsf_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_nsf_plugin;
}



