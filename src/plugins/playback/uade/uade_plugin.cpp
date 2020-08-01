#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <uade/uade.h>
#include <uade/eagleplayer.h>
#include <stdlib.h>
#include <string.h>

//static char s_player_name[1024]; // hack

#include <thread>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void get_file_stem(char* dest, const char* path) {
    const char* end_path = nullptr;

	for(size_t i = strlen(path) - 1;  i > 0; i--) {
		if (path[i] == '/') {
            end_path = &path[i+1];
            break;
		}
    }

    strcpy(dest, end_path);

    for(size_t i = strlen(dest) - 1;  i > 0; i--) {
        if (dest[i] == '.') {
            dest[i] = 0;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::thread* s_uade_thread = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void uade_run_thread(void (*f)(void*), void* data) {
    s_uade_thread = new std::thread(f, data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void uade_wait_thread() {
    if (s_uade_thread) {
        s_uade_thread->join();
    }

    delete s_uade_thread;
    s_uade_thread = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct uade_state* create_uade_state(int spawn) {
	struct uade_config* config = uade_new_config();
	uade_config_set_option(config, UC_ONE_SUBSONG, NULL);
	uade_config_set_option(config, UC_IGNORE_PLAYER_CHECK, NULL);
	uade_config_set_option(config, UC_NO_EP_END, NULL);
	uade_config_set_option(config, UC_FREQUENCY, "48000");

	//uade_config_set_option(config, UC_VERBOSE, "true");

	uade_config_set_option(config, UC_BASE_DIR, "bin/plugins/uade");
	uade_state* state = uade_new_state(config, spawn);

    free(config);

    return state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct UadePlugin {
    const HippoMetadataAPI* metadata_api = nullptr;
	struct uade_state* state = nullptr;
	bool closing_down;
} UadePlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Fix me

static const char* uade_supported_extensions() {
	return "ahx,hvl,ml";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult uade_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    enum HippoProbeResult supported = HippoProbeResult_Unsupported;

    struct uade_detection_info detect_info;
    struct uade_state* state = create_uade_state(0);

    if (uade_analyze_eagleplayer(&detect_info, data, data_size, filename, strlen(filename), state) >= 0) {
        supported = HippoProbeResult_Supported;
    }

    uade_cleanup_state(state, 0);

    //supported = HippoProbeResult_Supported;

	return supported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* uade_create(const struct HippoServiceAPI* services) {
	UadePlugin* plugin = new UadePlugin;
    plugin->metadata_api = HippoServiceAPI_get_metadata_api(services, HIPPO_METADATA_API_VERSION);

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_open(void* user_data, const char* buffer, int subsong) {
	UadePlugin* plugin = (UadePlugin*)user_data;

	plugin->state = create_uade_state(1);

	if (uade_play(buffer, subsong, plugin->state) == 1) {
		return 1;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_close(void* user_data) {
	UadePlugin* plugin = (UadePlugin*)user_data;

	if (plugin->state) {
		uade_stop(plugin->state);
		uade_cleanup_state(plugin->state, 1);
	}

	plugin->state = 0;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_destroy(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_read_data(void* user_data, void* dest, uint32_t max_samples) {
	UadePlugin* plugin = (UadePlugin*)user_data;

	int16_t data[480 * 2] = { 0 };

	int rc = uade_read(data, 480 * 2, plugin->state);
	(void)rc;

	float* new_dest = (float*)dest;

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < 480 * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return 480;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_metadata(const char* filename, const HippoServiceAPI* service_api) {
	auto state = create_uade_state(1);

    // TODO: Use io functions
	if (uade_play(filename, -1, state) != 1) {
	    uade_stop(state);
        uade_cleanup_state(state, 0);
	    return -1;
	}

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    const struct uade_song_info* song_info = uade_get_song_info(state);
    float length = (float)song_info->duration;

    char title[1024] = { 0 };

    if (strlen(song_info->modulename) > 1) {
        strcpy(title, song_info->modulename);
    } else {
        get_file_stem(title, filename);
    }

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, song_info->playername);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, length);

    int sub_min = song_info->subsongs.min;
    int sub_max = song_info->subsongs.max;

    if ((sub_max - sub_min) > 1) {
        for (int i = sub_min; i <= sub_max; ++i) {
            char subsong_name[1024] = { 0 };
            sprintf(subsong_name, "%s (%d/%d)", title, i, sub_max);
            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
        }
    }

	uade_stop(state);
    uade_cleanup_state(state, 0);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void uade_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_uade_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"UADE",
	"0.0.1",
	uade_probe_can_play,
	uade_supported_extensions,
	uade_create,
	uade_destroy,
	uade_event,
	uade_open,
	uade_close,
	uade_read_data,
	uade_plugin_seek,
	uade_metadata,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
	HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
		return &g_uade_plugin;
	}
}

