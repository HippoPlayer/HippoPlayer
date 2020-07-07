#include <HippoPlugin.h>
#include <stdlib.h>
#include <string.h>
#include <adplug.h>
#include <emuopl.h>
#include <silentopl.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a adplug plugin used as a reference and not inteded to do anything

struct AdplugPlugin {
    //AdplugPlugin() : emu_opl(48000, true, true) { }
    AdplugPlugin() : emu_opl(48000, true, false) { }
    CPlayer* player = nullptr;
    CEmuopl emu_opl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* adplug_supported_extensions() {
    // TODO: Fixme
	return "a2m";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult adplug_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    printf("adplug_probe_can_play\n");
    // TODO: Provide provide custom FILE api so we can read from memory
    CSilentopl silent;
    CPlayer* p = CAdPlug::factory(filename, &silent);

    printf("player %p\n", p);

    if (!p) {
        return HippoProbeResult_Unsupported;
    } else {
	    return HippoProbeResult_Supported;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* adplug_create(const struct HippoServiceAPI* services) {
	AdplugPlugin* plugin = new AdplugPlugin;

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_open(void* user_data, const char* url) {
	AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    // TODO: File io api
    plugin->player = CAdPlug::factory(url, &plugin->emu_opl);

    if (!plugin->player) {
        return 0;
    }

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_close(void* user_data) {
	AdplugPlugin* plugin = (AdplugPlugin*)user_data;

	delete plugin->player;
	delete plugin;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_destroy(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline int min_t(int a, int b) {
    return a < b ? a : b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_read_data(void* user_data, void* dest, uint32_t max_size) {
	AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    int toadd = 0;
    int i = 0;
    int towrite = 0;
    char* sndbufpos = nullptr;
	const int buffer_size = 512;
	const int sampsize = 4; // 16-bit & stereo
	const int freq = 48000;
	char sndbuf[buffer_size * sampsize * 2] = { 0 };

	float* new_dest = (float*)dest;

    // fill sound buffer
    towrite = buffer_size * 2;
    sndbufpos = sndbuf;

    printf("start....\n");

    while (towrite > 0) {
        while (toadd < 0) {
            toadd += freq;
            plugin->player->update();
            printf("update %f\n", plugin->player->getrefresh());
        }
        int t0 = (int)(toadd / plugin->player->getrefresh() + 4) & ~3;
        i = min_t(towrite, (int)(toadd / plugin->player->getrefresh() + 4) & ~3);
        printf("i %d - %d\n", i, t0);
        plugin->emu_opl.update((short *)sndbufpos, i);
        sndbufpos += i * sampsize;
        towrite -= i;
        printf("to write %d\n", towrite);
        toadd -= (int)(plugin->player->getrefresh() * i);
    }

    printf("end--\n");

	const float scale = 1.0f / 32767.0f;

	int16_t* t = (int16_t*)&sndbuf[0];

	for (i = 0; i < buffer_size * 2; ++i) {
	    //uint32_t fetch_offset = uint32_t(i) & ~1;
		//new_dest[i] = ((float)t[fetch_offset]) * scale;
		new_dest[i] = ((float)t[i / 2]) * scale;
	}

	return buffer_size * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_metadata(const char* url, const HippoServiceAPI* service_api) {
    // TODO: Provide provide custom FILE api so we can read from memory
    CSilentopl silent;
    CPlayer* p = CAdPlug::factory(url, &silent);

    if (!p) {
        return -1;
    }

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);
    HippoMetadataId index = HippoMetadata_create_url(metadata_api, url);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, p->gettitle().c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, p->gettype().c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, p->getauthor().c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, p->getdesc().c_str());

    // TODO: Sub-song support
    // TODO: This function is quite heavy (it will play the song to the end to figure out the length) maybe
    //       We should do this async instead and update the length later?
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, p->songlength());

	for (int i = 0, c = p->getinstruments(); i < c; ++i) {
    	HippoMetadata_add_instrument(metadata_api, index, p->getinstrument(i).c_str());
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void adplug_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)len;
    (void)data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_adplug_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"adplug",
	"0.0.1",
	adplug_probe_can_play,
	adplug_supported_extensions,
	adplug_create,
	adplug_destroy,
	adplug_event,
	adplug_open,
	adplug_close,
	adplug_read_data,
	adplug_plugin_seek,
	adplug_metadata,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_adplug_plugin;
}



