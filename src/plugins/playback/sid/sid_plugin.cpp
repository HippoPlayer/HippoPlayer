#include <HippoPlugin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sidplayfp/sidplayfp.h>
#include <sidplayfp/SidTuneInfo.h>
#include <sidplayfp/SidTune.h>
#include <sidplayfp/SidInfo.h>
#include <builders/residfp-builder/residfp.h>

#ifndef _WIN32
#include <libgen.h>
#else
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

const int FREQ = 48000;
const int FRAME_SIZE = (FREQ * 2) / 100;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SidReplayerData {
    sidplayfp engine;
	ReSIDfpBuilder* rs = nullptr;
	SidTune* tune = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I haven't found any way to probe the MDX file so have to do with extension compare :/

enum HippoProbeResult sid_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	const char* point = 0;

	if ((point = strrchr(filename,'.')) != NULL) {
		if (strcasecmp(point,".sid") == 0 || (strcasecmp(point,".psid") == 0)) {
			return HippoProbeResult_Supported;
		}
	}

	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* sid_supported_extensions() {
	return "sid,psid";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* sid_create(const struct HippoServiceAPI* service_api) {
	SidReplayerData* data = new SidReplayerData();

    data->engine.setRoms(nullptr, nullptr, nullptr);

	data->rs = new ReSIDfpBuilder("hippo_player");
    // Get the number of SIDs supported by the engine
    unsigned int max_sids = data->engine.info().maxsids();

	// Create SID emulators
    data->rs->create(max_sids);

    // Check if builder is ok
    if (!data->rs->getStatus()) {
        printf("SidPlugin error %s\n", data->rs->error());
        delete data->rs;
        delete data;
        return 0;
    }

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_destroy(void* user_data) {
	struct SidReplayerData* t = (struct SidReplayerData*)user_data;
	t->engine.stop();
	//delete t;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_open(void* user_data, const char* buffer) {
	SidReplayerData* data = (SidReplayerData*)user_data;

	SidTune* tune = new SidTune(buffer);

    // Check if the tune is valid
    if (!tune->getStatus()) {
        printf("SidPlugin: tune status %s\n", tune->statusString());
        return -1;
    }

    data->tune = tune;

    // Select default song
    tune->selectSong(0);

    // Configure the engine
    SidConfig cfg;
    cfg.frequency = FREQ;
    cfg.samplingMethod = SidConfig::INTERPOLATE;
    cfg.fastSampling = false;
    cfg.playback = SidConfig::MONO;
    cfg.sidEmulation = data->rs;
    cfg.defaultSidModel = SidConfig::MOS8580;

    if (!data->engine.config(cfg)) {
        printf("Engine error %s\n", data->engine.error());
        return 0;
    }

    // Load tune into engine
    if (!data->engine.load(data->tune)) {
        printf("Engine error %s\n", data->engine.error());
        return 0;
    }

    printf("sid_open ok\n");

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_close(void* user_data) {
	struct SidReplayerData* plugin = (struct SidReplayerData*)user_data;

	if (plugin->tune) {
	    plugin->engine.stop();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_read_data(void* user_data, void* dest, uint32_t max_samples) {
	SidReplayerData* data = (SidReplayerData*)user_data;
	float* output = (float*)dest;

	int16_t temp_data[FRAME_SIZE * 2] = { 0 };

	data->engine.play(temp_data, FRAME_SIZE / 2);

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < FRAME_SIZE / 2; ++i) {
		const float v = ((float)temp_data[i]) * scale;
		*output++ = v;
		*output++ = v;
	}

	return FRAME_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int sid_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void sid_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_sid_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"SID",
	"0.0.1",
	sid_probe_can_play,
	sid_supported_extensions,
	sid_create,
	sid_destroy,
	sid_event,
	sid_open,
	sid_close,
	sid_read_data,
	sid_seek,
	NULL,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_sid_plugin;
}


