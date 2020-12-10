#include <HippoPlugin.h>
#include <adplug.h>
#include <emuopl.h>
#include <silentopl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wemuopl.h>
#include <assert.h>

extern "C" {
    HippoLogAPI* g_hp_log = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void get_file_stem(char* dest, const char* path) {
    const char* end_path = nullptr;

    for (size_t i = strlen(path) - 1; i > 0; i--) {
        if (path[i] == '/') {
            end_path = &path[i + 1];
            break;
        }
    }

    strcpy(dest, end_path);

    for (size_t i = strlen(dest) - 1; i > 0; i--) {
        if (dest[i] == '.') {
            dest[i] = 0;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a adplug plugin used as a reference and not inteded to do anything

struct AdplugPlugin {
    AdplugPlugin() : emu_opl(48000, true, true) {
    }
    CPlayer* player = nullptr;
    CWemuopl emu_opl;
    int to_add = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* adplug_supported_extensions() {
    // TODO: Fixme
    return "a2m";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult adplug_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                            uint64_t total_size) {
    // TODO: Provide provide custom FILE api so we can read from memory
    CSilentopl silent;
    CPlayer* p = CAdPlug::factory(filename, &silent);

    if (!p) {
        hp_debug("Unsupported: %s", filename);
        return HippoProbeResult_Unsupported;
    } else {
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* adplug_create(const struct HippoServiceAPI* services) {
    AdplugPlugin* plugin = new AdplugPlugin;

    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_open(void* user_data, const char* url, int subsong) {
    AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    // TODO: File io api
    plugin->player = CAdPlug::factory(url, &plugin->emu_opl);

    if (!plugin->player) {
        hp_error("Unable to playing: %s\n", url);
        return 0;
    }

    plugin->player->rewind(subsong);

    hp_info("Started to play: %s (subsong %d)", url, subsong);

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
static inline int max_t(int a, int b) {
    return a > b ? a : b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_read_data(void* user_data, void* dest, uint32_t samples_to_read) {
    AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    int i = 0;
    int towrite = 0;
    char* sndbufpos = nullptr;
    const int buffer_size = 2048;
    const int sampsize = 4;  // 16-bit & stereo
    const int freq = 48000;
    char sndbuf[buffer_size * sampsize * 2] = {0};

    samples_to_read = buffer_size;
    float* new_dest = (float*)dest;

    // fill sound buffer
    towrite = buffer_size;
    sndbufpos = sndbuf;

    while (towrite > 0) {
        while (plugin->to_add < 0) {
            plugin->to_add += freq;
            plugin->player->update();
        }
        i = min_t(towrite, (int)(plugin->to_add / plugin->player->getrefresh() + 4) & ~3);
        plugin->emu_opl.update((short*)sndbufpos, i);
        sndbufpos += i * sampsize;
        towrite -= i;
        i = (int)(plugin->player->getrefresh() * i);
        plugin->to_add -= (int)max_t(1, i);
    }

    const float scale = 1.0f / 32767.0f;

    int16_t* t = (int16_t*)&sndbuf[0];

    for (i = 0; i < samples_to_read * 2; ++i) {
        new_dest[i] = ((float)t[i]) * scale;
    }

    return samples_to_read * 2;
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
        hp_error("Unable to get metadata for %s\n", url);
        return -1;
    }

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);
    HippoMetadataId index = HippoMetadata_create_url(metadata_api, url);

    char title[1024] = {0};

    const char* meta_title = p->gettitle().c_str();

    // make sure the title actually conists of some chars
    if (strlen(meta_title) < 2) {
        get_file_stem(title, url);
    } else {
        strcpy(title, meta_title);
    }

    hp_info("Updating metadata for %s", url);

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, p->gettype().c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, p->getauthor().c_str());
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, p->getdesc().c_str());

    // TODO: This function is quite heavy (it will play the song to the end to
    // figure out the length) maybe
    //       We should do this async instead and update the length later?
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, p->songlength() / 1000);

    for (int i = 0, c = p->getinstruments(); i < c; ++i) {
        HippoMetadata_add_instrument(metadata_api, index, p->getinstrument(i).c_str());
    }

    const int subsongs_count = p->getsubsongs();

    if (subsongs_count > 1) {
        for (int i = 0; i < subsongs_count; ++i) {
            char subsong_name[2048] = {0};
            auto len = p->songlength(i) / 1000;
            // Only add songs if they actually have a length
            if (len > 0) {
                sprintf(subsong_name, "%s (%d/%d)", title, i + 1, subsongs_count);
                HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, len);
            }
        }
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

static void adplug_set_log(struct HippoLogAPI* log) {
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void AdPlug_LogWrite(const char* fmt, ...) {
    /*
    char buffer[2048];

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);

    auto len = strlen(buffer);
    // remove newline as we auto-add it in the log macro
    if (len > 3) {
        buffer[len - 3] = '\0';
    }

    hp_trace("%s", buffer);
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_adplug_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "adplug",
    "1.0.0",
    "adplug 2.3.3",
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
    adplug_set_log,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &g_adplug_plugin;
}
