#include <HippoPlugin.h>
#include <HippoMessages.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include "libsc68/sc68/sc68.h"
#include "file68/sc68/file68.h"
}

#define SAMPLE_RATE 48000
#define CHANNELS 2

HippoLogAPI* g_hp_log = NULL;
static const struct HippoIoAPI* g_io_api = 0;

typedef struct Sc68Plugin {
    sc68_t* instance;
} Sc68Plugin;

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

static const char* sc68_plugin_supported_extensions() {
	return "sc68,snd,sndh";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Should use mutex here

static bool s_init_done = false;
static sc68_init_t s_init;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
static void msg(const int cat, void* cookie, const char * fmt, va_list list) {
    vfprintf(stdout,fmt,list);
    fflush(stdout);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void init() {
    if (s_init_done) {
        return;
    }

    static char appname[] = "hippo_player";
    static char* argv[] = { appname };

    s_init.argc = sizeof(argv)/sizeof(*argv);
    s_init.argv = argv;
#ifdef DEBUG
    s_init.msg_handler = (sc68_msg_t)msg;
#endif

    sc68_init(&s_init);
    s_init_done = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult sc68_plugin_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    init();

    if (file68_verify_header((void*)data, (int)data_size) == -1) {
        hp_debug("Unsupported: %s", filename);
	    return HippoProbeResult_Unsupported;
    }

    hp_info("Supported: %s", filename);
	return HippoProbeResult_Supported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_metadata(const char* filename, const HippoServiceAPI* service_api) {
    sc68_music_info_t info = { 0 };
    void* data = 0;
    uint64_t size = 0;

    init();

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to file-io open %s", filename);
        return res;
    }

    sc68_create_t create = { 0 };
    create.log2mem = 19;
    create.name = "test";
    create.sampling_rate = SAMPLE_RATE;
    sc68_t* inst = sc68_create(&create);

    if (sc68_load_mem(inst, data, (int)size) < 0) {
        hp_error("Unable to load_mem %s", filename);
        return -1;
    }

    int ret = sc68_music_info(inst, &info, 1, 0);
    int length = info.trk.time_ms;
    (void)ret;

    int subsong_count = info.tracks;

    char title[4096] = { 0 };

    if (!info.title) {
        get_file_stem(title, filename);
    } else {
        strcpy(title, info.title);
    }

    hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, info.format);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AuthoringToolTag, info.converter);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, info.artist);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_DateTag, info.year);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, info.ripper);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AlbumTag, info.album);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_GenreTag, info.genre);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, length / 1000);

    if (subsong_count > 1) {
        for (int i = 0; i < subsong_count; ++i) {
            char subsong_name[1024] = { 0 };
            sc68_music_info_t info = { 0 };

            int ret = sc68_music_info(inst, &info, i + 1, 0);

            if (ret && info.title) {
                sprintf(subsong_name, "%s - %s (%d/%d)", title, info.title, i + 1, subsong_count);
            } else {
                sprintf(subsong_name, "%s (%d/%d)", title, i + 1, subsong_count);
            }

            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, info.trk.time_ms / 1000);
        }
    }

    sc68_destroy(inst);

    // Make sure to free the buffer before we leave
    HippoIo_free_file_to_memory(io_api, data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* sc68_plugin_create(const struct HippoServiceAPI* service_api) {
    init();

    sc68_create_t create = { 0 };
    create.log2mem = 19;
    create.name = "test";
    create.sampling_rate = SAMPLE_RATE;
    sc68_t* instance = sc68_create(&create);

    // TODO: Proper error handling
    if (!instance) {
        hp_error("Failed to create");
        return nullptr;
    }

    Sc68Plugin* plugin = new Sc68Plugin;
    plugin->instance = instance;

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_open(void* user_data, const char* buffer, int subsong) {
    uint64_t size = 0;
    void* load_data = nullptr;
	Sc68Plugin* data = (Sc68Plugin*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data, buffer, &load_data, &size);

    if (res < 0) {
        hp_error("Failed to io-open %s", buffer);
        return res;
    }

    if (sc68_load_mem(data->instance, load_data, (int)size) < 0) {
        hp_error("Unable to load_mem %s", buffer);
        return -1;
    }

    if (sc68_process(data->instance, 0, 0) == SC68_ERROR) {
        hp_error("Failed to process %s", buffer);
        return -1;
    }

    hp_info("Starting to play %s (subsong %d)", buffer, subsong);

    sc68_play(data->instance, subsong, 0);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_close(void* user_data) {
	Sc68Plugin* plugin = (Sc68Plugin*)user_data;

    if (plugin->instance) {
        sc68_destroy(plugin->instance);
    }

    delete plugin;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_destroy(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
	Sc68Plugin* plugin = (Sc68Plugin*)user_data;

	int16_t data[1024 * 2] = { 0 };
    int n = samples_to_read;

    // TODO: Handle error
    int code = sc68_process(plugin->instance, data, &n);
    (void)code;

	float* new_dest = (float*)dest;
	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < samples_to_read * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sc68_plugin_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)len;
    (void)data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sc68_plugin_set_log(struct HippoLogAPI* log) { g_hp_log = log; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_sc68_plugin_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"sc68",
	"0.0.1",
	"libsc68 svn 2020-04-02",
	sc68_plugin_probe_can_play,
	sc68_plugin_supported_extensions,
	sc68_plugin_create,
	sc68_plugin_destroy,
	sc68_plugin_event,
	sc68_plugin_open,
	sc68_plugin_close,
	sc68_plugin_read_data,
	sc68_plugin_plugin_seek,
	sc68_plugin_metadata,
    sc68_plugin_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_sc68_plugin_plugin;
}

