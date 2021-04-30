#include <HippoPlugin.h>
#include <adplug.h>
#include <assert.h>
#include <emuopl.h>
#include <silentopl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wemuopl.h>
#include <nemuopl.h>
#include <kemuopl.h>
#include <surroundopl.h>

extern "C" {
HippoLogAPI* g_hp_log = NULL;
}

#define PLUGIN_NAME "adplug"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ID_SAMPLE_RATE "SampleRate"
#define ID_CHANNELS "Channels"
#define ID_ADLIB_CORE "AdlibCore"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clang-format off

static const HSIntegerRangeValue s_sample_rate[] = {
    {"Default", 49716},
    {"6000", 6000},
    {"8000", 6000},
    {"11025", 11025},
    {"22050", 22050},
    {"32000", 32000},
    {"44100", 44100},
    {"48000", 48000},
    {"49716", 49716},
    {"82000", 82000},
    {"96000", 96000},
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const HSIntegerRangeValue s_channels[] = {
    {"Default", 0},
    {"Mono", 1},
    {"Stereo", 2},
    {"Quad", 3},
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Channels {
    Default,
    Mono,
    Stereo,
    Surround,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const HSIntegerRangeValue s_core[] = {
    {"Harekiet's", 0},
    {"Ken Silverman's", 1},
    {"Jarek Burczynski's", 2},
    {"Nuked OPL3", 3},
};

enum class Core {
    Harekiet,
    Ken,
    Jarek,
    Nuked,
};

// clang-format on
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HSSetting s_settings[] = {
    HSIntValue_DescRange(ID_SAMPLE_RATE, "Sample rate",
                         "Default (recommended) uses the sample rate by the output device", 49716, s_sample_rate),
    //HSIntValue_DescRange(ID_CHANNELS, "Channels",
    //                     "Default (recommended) uses the number of channels the current song has.", 0, s_channels),
    HSIntValue_DescRange(ID_ADLIB_CORE, "Adlib Core", "Select the Adlib core to be used", 0, s_core),
};

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
    COPLprops* emu_core = nullptr;
    CPlayer* player = nullptr;
    int to_add = 0;
    int sample_rate;
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

static COPLprops* create_adlib(Core core, uint16_t sample_rate, bool stereo) {
    struct COPLprops* emu_core = new COPLprops;

    emu_core->use16bit = true;
    emu_core->stereo = stereo;

    switch (core) {
        case Core::Nuked: {
            printf("created neke %d\n", sample_rate);
            emu_core->opl = new CNemuopl(sample_rate);
            emu_core->stereo = true;
            break;
        }
        case Core::Jarek: {
            printf("created jake %d\n", sample_rate);
            emu_core->opl = new CEmuopl(sample_rate, true, stereo);
            break;
        }
        case Core::Ken: {
            printf("created keyn %d\n", sample_rate);
            emu_core->opl = new CKemuopl(sample_rate, true, stereo);
            break;
        }
        case Core::Harekiet: {
            printf("create hare %d\n", sample_rate);
            emu_core->opl = new CWemuopl(sample_rate, true, stereo);
        }
    }

    return emu_core;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static COPLprops* create_adlib_surround(Core core, uint16_t srate) {
    COPLprops* left = create_adlib(core, srate, false);
    COPLprops* right = create_adlib(core, srate, false);

    COPLprops* emu_core = new COPLprops;
    emu_core->use16bit = true;
    emu_core->stereo = true;

    emu_core->opl = new CSurroundopl(left, right, true);

    return emu_core;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

COPLprops* create_core(AdplugPlugin* plugin, const struct HippoSettingsAPI* api) {

    COPLprops* core = nullptr;
    int sample_rate = 49716;
    int channels = (int)Channels::Stereo;
    int core_id = (int)Core::Harekiet;

    HippoSettings_get_int(api, nullptr, ID_SAMPLE_RATE, &sample_rate);
    HippoSettings_get_int(api, nullptr, ID_CHANNELS, &channels);
    HippoSettings_get_int(api, nullptr, ID_ADLIB_CORE, &core_id);

    switch ((Channels)channels) {
        case Channels::Surround: {
            core = create_adlib_surround((Core)core_id, sample_rate);
            break;
        };
        case Channels::Mono: {
            core = create_adlib((Core)core_id, sample_rate, false);
            break;
        };

        default:
        case Channels::Default:
        case Channels::Stereo: {
            core = create_adlib((Core)core_id, sample_rate, true);
            break;
        };
    }

    plugin->sample_rate = sample_rate;

    return core;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_open(void* user_data, const char* url, int subsong, const struct HippoSettingsAPI* settings) {
    AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    plugin->emu_core = create_core(plugin, settings);

    // TODO: File io api
    plugin->player = CAdPlug::factory(url, plugin->emu_core->opl);

    if (!plugin->player) {
        hp_error("Unable to playing: %s\n", url);
        return 0;
    }

    plugin->emu_core->opl->init();

    plugin->player->rewind(subsong);

    hp_info("Started to play: %s (subsong %d)", url, subsong);

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int adplug_close(void* user_data) {
    AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    if (plugin->emu_core) {
        delete plugin->emu_core->opl;
        delete plugin->emu_core;
    }

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

static HippoReadInfo adplug_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                      uint32_t native_sample_rate) {
    AdplugPlugin* plugin = (AdplugPlugin*)user_data;

    int i = 0;
    int towrite = 0;
    char* sndbufpos = (char*)dest;
    const int buffer_size = 2048;
    int sampsize = 4;  // 16-bit & stereo

    if (!plugin->emu_core->stereo) {
        sampsize = 2;
    }

    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, buffer_size);

    // fill sound buffer
    towrite = buffer_size;

    while (towrite > 0) {
        while (plugin->to_add < 0) {
            plugin->to_add += plugin->sample_rate;
            plugin->player->update();
        }
        i = min_t(towrite, (int)(plugin->to_add / plugin->player->getrefresh() + 4) & ~3);
        plugin->emu_core->opl->update((short*)sndbufpos, i);
        sndbufpos += i * sampsize;
        towrite -= i;
        i = (int)(plugin->player->getrefresh() * i);
        plugin->to_add -= (int)max_t(1, i);
    }

    return HippoReadInfo { (uint16_t)plugin->sample_rate, samples_to_read, 2, HippoOutputType_s16 };
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

static void adplug_static_init(struct HippoLogAPI* log, const struct HippoServiceAPI* service) {
    g_hp_log = log;

    auto settings_api = HippoServiceAPI_get_settings_api(service, HIPPO_SETTINGS_API_VERSION);

    if (HippoSettings_register_settings(settings_api, PLUGIN_NAME, s_settings) != HippoSettingsError_Ok) {
        // hp_error("Unable to register settings, error: %s", HippoSettings_get_last_error(api));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoSettingsUpdate adplug_settings_updated(void* user_data, const HippoSettingsAPI* settings) {
    (void)user_data;
    (void)settings;
    // TODO: Support seeking
    return HippoSettingsUpdate_RequireSongRestart;
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
    PLUGIN_NAME,
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
    adplug_static_init,
    adplug_settings_updated,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &g_adplug_plugin;
}
