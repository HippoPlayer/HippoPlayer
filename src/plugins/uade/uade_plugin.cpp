#include "../../plugin_api/HippoPlugin.h"
#include <uade/uade.h>
#include <uade/eagleplayer.h>
#include <stdlib.h>
#include <string.h>

static char s_player_name[1024]; // hack

#include <thread>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::thread* s_uade_thread = nullptr;

extern "C" void uade_run_thread(void (*f)(void*), void* data) {
    printf("Starting thread\n");
    s_uade_thread = new std::thread(f, data);
}

extern "C" void uade_wait_thread() {
    if (s_uade_thread) {
        s_uade_thread->join();
    }

    printf("ended thread\n");

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
	struct uade_state* state = nullptr;
	bool closing_down;
} UadePlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Fix me

static const char* uade_supported_extensions() {
	return "ahx,hvl,ml";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getFileNameFromPath(const char* path)
{
   for(size_t i = strlen(path) - 1;  i > 0; i--)
   {
      if (path[i] == '/')
      {
         return &path[i+1];
      }
   }

   return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static const char* uade_track_info(void* user_data) {
	UadePlugin* plugin = (UadePlugin*)user_data;

    const struct uade_song_info* song_info = uade_get_song_info(plugin->state);

    if (!song_info) {
    	return "unknown";
    }

    if (song_info->modulename[0] == 0) {
		sprintf(s_player_name, "%s (%s)", getFileNameFromPath(song_info->modulefname), song_info->playername);
    } else {
		sprintf(s_player_name, "%s (%s)", song_info->modulename, song_info->playername);
    }

	return s_player_name;
}
*/

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

static void* uade_create(struct HippoServiceAPI* services) {
	UadePlugin* plugin = new UadePlugin;

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int uade_open(void* user_data, const char* buffer) {
	UadePlugin* plugin = (UadePlugin*)user_data;

	plugin->state = create_uade_state(1);

	if (uade_play(buffer, -1, plugin->state) == 1) {
		printf("starting play\n");
		return 1;
	}

	return 0;
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

	const float scale = 1.0f / 32768.0f;

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

/*
static int uade_length(void* user_data) {
	UadePlugin* plugin = (UadePlugin*)user_data;

	if (!plugin->state) {
	    return -10;
	}

    const struct uade_song_info* song_info = uade_get_song_info(plugin->state);

    if (!song_info) {
        return -10;
    }

    printf("uade: length %f\n", song_info->duration);

    if (song_info->duration > 0.0) {
        return (int)song_info->duration;
    }

	return -10;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_uade_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	uade_probe_can_play,
	uade_supported_extensions,
	uade_create,
	uade_destroy,
	uade_open,
	uade_close,
	uade_read_data,
	uade_plugin_seek,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
	HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
		return &g_uade_plugin;
	}
}

