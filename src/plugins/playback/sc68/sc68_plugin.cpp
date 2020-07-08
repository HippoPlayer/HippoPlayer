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

static const struct HippoIoAPI* g_io_api = 0;

typedef struct Sc68Plugin {
    sc68_t* instance;
} Sc68Plugin;

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
	    return HippoProbeResult_Unsupported;
    }

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
        return res;
    }

    sc68_create_t create = { 0 };
    create.log2mem = 19;
    create.name = "test";
    create.sampling_rate = SAMPLE_RATE;
    sc68_t* inst = sc68_create(&create);

    // TODO: Proper error handling
    if (sc68_load_mem(inst, data, (int)size) < 0) {
        return -1;
    }

    int ret = sc68_music_info(inst, &info, 1, 0);
    int length = info.trk.time_ms;
    (void)ret;

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, info.title ? info.title : ""); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, info.format); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AuthoringToolTag, info.converter); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, info.artist); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_DateTag, info.year); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, info.ripper); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_AlbumTag, info.album); 
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_GenreTag, info.genre); 
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, length / 1000); 

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
        printf("sc68_plugin: Failed to create\n");
        return nullptr;
    }

    Sc68Plugin* plugin = new Sc68Plugin;
    plugin->instance = instance;

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sc68_plugin_open(void* user_data, const char* buffer) {
    uint64_t size = 0;
    void* load_data = nullptr;
	Sc68Plugin* data = (Sc68Plugin*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data, buffer, &load_data, &size);

    if (res < 0) {
        printf("sc68_plugin: Failed load file\n");
        return res;
    }

    // TODO: Proper error handling
    if (sc68_load_mem(data->instance, load_data, (int)size) < 0) {
        printf("sc68_plugin: Failed load\n");
        return -1;
    }

    if (sc68_process(data->instance, 0, 0) == SC68_ERROR) {
        printf("sc68_plugin: Failed process\n");
        return -1;
    }

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

static int sc68_plugin_read_data(void* user_data, void* dest, uint32_t max_size) {
	Sc68Plugin* plugin = (Sc68Plugin*)user_data;

	int16_t data[800 * 2] = { 0 };
    int n = sizeof(data) >> 2;

    // TODO: Handle error
    int code = sc68_process(plugin->instance, data, &n);
    (void)code;

	float* new_dest = (float*)dest;
	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < n * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return n * 2;
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

static HippoPlaybackPlugin g_sc68_plugin_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"sc68_plugin",
	"0.0.1",
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
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_sc68_plugin_plugin;
}





