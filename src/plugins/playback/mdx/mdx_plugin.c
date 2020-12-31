#include <HippoPlugin.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/mdxmini.h"

#ifndef _WIN32
#include <libgen.h>
#else
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define MDX_SAMPLE_RATE 48000
#define FRAME_SIZE 1024

HippoLogAPI* g_hp_log = NULL;

char* sj2utf8(const char* input);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char* get_dirname(const char* src, char* dir_output) {
    int len = (int)strlen(src);

    for (int i = len - 1; i != 0; --i) {
        if (src[i] == '/' || src[i] == '\\') {
            strncpy(dir_output, src, i);
            return dir_output;
        }
    }

    return dir_output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a mdx_plugin plugin used as a reference and not inteded to do anything

typedef struct MDXPlugin {
    t_mdxmini mdx_tune;
} MDXPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* mdx_plugin_supported_extensions() {
    return "mdx";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I haven't found any way to probe the MDX file so have to do with extension compare :/

enum HippoProbeResult mdx_plugin_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                                uint64_t total_size) {
    const char* point = 0;

    if ((point = strrchr(filename, '.')) != NULL) {
        if (strcasecmp(point, ".mdx") == 0) {
            hp_info("Supported: %s", filename);
            return HippoProbeResult_Supported;
        }
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mdx_plugin_create(const struct HippoServiceAPI* services) {
    MDXPlugin* plugin = malloc(sizeof(MDXPlugin));
    memset(plugin, 0, sizeof(MDXPlugin));

    mdx_set_rate(MDX_SAMPLE_RATE);

    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_plugin_open(void* user_data, const char* buffer, int subsong) {
    char dir_name[4096];
    MDXPlugin* plugin = (MDXPlugin*)user_data;

    (void)subsong;

    get_dirname(buffer, dir_name);

    if (mdx_open(&plugin->mdx_tune, (char*)buffer, dir_name) >= 0) {
        hp_info("Unable to open: %s", buffer);
        return 1;
    }

    hp_info("Starting to play %s", buffer);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_plugin_close(void* user_data) {
    MDXPlugin* plugin = (MDXPlugin*)user_data;

    mdx_close(&plugin->mdx_tune);
    memset(&plugin->mdx_tune, 0, sizeof(t_mdxmini));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_plugin_destroy(void* user_data) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo mdx_plugin_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                          uint32_t native_sample_rate) {
    MDXPlugin* plugin = (MDXPlugin*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

    mdx_calc_sample(&plugin->mdx_tune, dest, (samples_to_read * 2) / 2);

    HippoReadInfo t = {MDX_SAMPLE_RATE, samples_to_read, 2, HippoOutputType_s16};

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_plugin_plugin_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_metadata(const char* filename, const HippoServiceAPI* service_api) {
    // TODO: Max buffer handling
    char title[1024];
    char dir_name[4096];
    t_mdxmini mdx_tune;

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    get_dirname(filename, dir_name);

    // TODO: Look into using file apis but support needs to be added to mdx

    if (mdx_open(&mdx_tune, (char*)filename, dir_name) < 0) {
        hp_error("Unable to open %s", filename);
        return -1;
    }

    hp_info("Updating metadata for %s", filename);

    mdx_get_title(&mdx_tune, title);
    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, (float)mdx_get_length(&mdx_tune));
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "MDX (X68000 Music)");

    mdx_close(&mdx_tune);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mdx_plugin_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)len;
    (void)data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void mdx_static_init(struct HippoLogAPI* log, const struct HippoServiceAPI* service) {
	(void)service;
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_mdx_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "mdx",
    "0.0.1",
    "mdxmini 1.0.0",
    mdx_plugin_probe_can_play,
    mdx_plugin_supported_extensions,
    mdx_plugin_create,
    mdx_plugin_destroy,
    mdx_plugin_event,
    mdx_plugin_open,
    mdx_plugin_close,
    mdx_plugin_read_data,
    mdx_plugin_plugin_seek,
    mdx_metadata,
    mdx_static_init,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &g_mdx_plugin;
}
