#include <HippoPlugin.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a dummy plugin used as a reference and not inteded to do anything

typedef struct DummyPlugin {
	int dummy;
} DummyPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* dummy_supported_extensions() {
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult dummy_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* dummy_create(const struct HippoServiceAPI* services) {
	DummyPlugin* plugin = malloc(sizeof(DummyPlugin));
	memset(plugin, 0, sizeof(DummyPlugin));

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int dummy_open(void* user_data, const char* buffer) {
	DummyPlugin* plugin = (DummyPlugin*)user_data;
	(void)plugin;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int dummy_close(void* user_data) {
	DummyPlugin* plugin = (DummyPlugin*)user_data;
	(void)plugin;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int dummy_destroy(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int dummy_read_data(void* user_data, void* dest, uint32_t max_size) {
	DummyPlugin* plugin = (DummyPlugin*)user_data;
	(void)plugin;

	int16_t data[480 * 2] = { 0 };

	float* new_dest = (float*)dest;

	const float scale = 1.0f / 32768.0f;

	for (int i = 0; i < 480 * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return 480;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int dummy_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void dummy_event(void* user_data, const struct HippoMessageDecode* message) {
    (void)user_data;
    (void)message;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_dummy_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"dummy",
	"0.0.1",
	dummy_probe_can_play,
	dummy_supported_extensions,
	dummy_create,
	dummy_destroy,
	dummy_event,
	dummy_open,
	dummy_close,
	dummy_read_data,
	dummy_plugin_seek,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_dummy_plugin;
}


