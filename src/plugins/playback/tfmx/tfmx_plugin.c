#include <HippoPlugin.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/tfmx.h"
#include "src/tfmx_iface.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* get_file_name_from_path(const char* path) {
    for (size_t i = strlen(path) - 1; i > 0; i--) {
        if (path[i] == '/') {
            // if file starts with "mdat." we return without the name
            if (memcmp(&path[i + 1], "mdat.", 5) == 0) {
                return &path[i + 6];
            } else {
                return &path[i + 1];
            }
        }
    }

    return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct TfmxReplayerData {
    void* tune;
} TfmxReplayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tfmx_supported_extensions(void* user_data) {
    return "tfmx,tfx";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult tfmx_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    if (strncmp("TFMX-SONG", (char*)data, 9)
        && strncmp("TFMX_SONG", (char*)data, 9)
        && strncasecmp("TFMXSONG", (char*)data, 8)
        && strncasecmp("TFMX ", (char*)data, 5)) {
        hp_info("Unsupported %s", filename);
        return HippoProbeResult_Unsupported;
    }

    hp_info("Supported %s", filename);
    return HippoProbeResult_Supported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* tfmx_create() {
    TfmxReplayerData* replayer;

    replayer = (TfmxReplayerData*)malloc(sizeof(struct TfmxReplayerData));
    memset(replayer, 0, sizeof(struct TfmxReplayerData));

    return replayer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_destroy(void* user_data) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_open(void* user_data, const char* buffer, int subsong) {
    TfmxReplayerData* plugin = (TfmxReplayerData*)user_data;
    (void)plugin;

    if (LoadTFMXFile((char*)buffer) != 0) {
        hp_error("Unable to open %s", buffer);
        return -1;
    }

    TFMXSetSubSong(subsong);
    TFMXRewind();

    hp_info("Starting to play %s (subsong %d)", buffer, subsong);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_close(void* user_data) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_read_data(void* user_data, void* dest, uint32_t max_samples) {
    int16_t temp_data[BUFSIZE] = {0};

    int block_size = (int)tfmx_get_block_size() / 2;

    assert(block_size < BUFSIZE);

    if (tfmx_try_to_make_block() >= 0) {
        tfmx_get_block(temp_data);
    }

    const float scale = 1.0f / 32767.0f;

    float* new_dest = (float*)dest;

    for (int i = 0; i < block_size; ++i) {
        new_dest[i] = ((float)temp_data[i]) * scale;
    }

    return block_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_metadata(const char* url, const HippoServiceAPI* service_api) {
    // TODO: Fix that tfmx can load from a buffer, always from file now because of the lib code

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    if (LoadTFMXFile((char*)url) != 0) {
        hp_error("Unable to get metadata for %s", url);
        return -1;
    }

    const char* title = get_file_name_from_path(url);

    hp_info("Updating metadata for %s", url);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, url);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "TFMX");
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, 0.0f);

    // text info data for TFMX is 40 * 6 but * 2 for some saftey
    char text_info[(40 * 6) * 2] = {0};
    tfmx_fill_text_info(text_info);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, text_info);

    int subsongs_count = TFMXGetSubSongs();

    if (subsongs_count > 1) {
        for (int i = 0; i < subsongs_count; ++i) {
            char subsong_name[1024] = {0};
            sprintf(subsong_name, "%s (%d/%d)", title, i + 1, subsongs_count);
            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tfmx_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)len;
    (void)data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tfmx_set_log(struct HippoLogAPI* log) {
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_tfmx_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "tfmx",
    "0.0.1",
    "",
    tfmx_probe_can_play,
    tfmx_supported_extensions,
    tfmx_create,
    tfmx_destroy,
    tfmx_event,
    tfmx_open,
    tfmx_close,
    tfmx_read_data,
    tfmx_seek,
    tfmx_metadata,
    tfmx_set_log,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &g_tfmx_plugin;
}
