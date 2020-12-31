#include <HippoPlugin.h>
#include <gme/gme.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREQ 48000
#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
    Music_Emu* song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* gme_supported_extensions() {
    return "gbs,spc";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* find_filename_start(const char* path, int* offset) {
    for (size_t i = strlen(path) - 1; i > 0; i--) {
        char c = path[i];
        if (c == '/' || c == '\\') {
            *offset = (int)(i + 1);
            return &path[i + 1];
        }
    }

    *offset = 0;
    return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* gme_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gme_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if (data->song) {
        gme_delete(data->song);
    }

    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Music_Emu* open_gme(const char* filename, const struct HippoIoAPI* io) {
    const char* error = nullptr;
    uint64_t size = 0;
    void* data;

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to open: %s", filename);
        return NULL;
    }

    Music_Emu* song = nullptr;

    if ((error = gme_open_data(data, (long)size, &song, FREQ))) {
        hp_warn("Internal error: %s\n", error);
    }

    HippoIo_free_file_to_memory(io, data);

    return song;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gme_open(void* user_data, const char* filename, int subsong) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if ((data->song = open_gme(filename, g_io_api)) == nullptr) {
        return -1;
    }

    gme_start_track(data->song, subsong);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gme_close_in(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
    gme_delete(data->song);
    data->song = NULL;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult gme_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                         uint64_t total_size) {
    const char* error = 0;
    Music_Emu* song;

    if ((error = gme_open_data(data, (long)data_size, &song, FREQ)) != nullptr) {
        // failed we try again with loading the full file

        // TODO: Use IO
        if ((error = gme_open_file(filename, &song, FREQ)) != nullptr) {
            hp_debug("Unsupported: %s (%s)", filename, error);
            return HippoProbeResult_Unsupported;
        }
    }

    gme_delete(song);
    hp_info("Supported: %s", filename);
    return HippoProbeResult_Supported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct CallbackData {
    void* dest;
    int max_count;
    int ret_count;
} CallbackData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo gme_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                   uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

    gme_play(data->song, samples_to_read, (short*)dest);

    return HippoReadInfo {
        FREQ,
        uint16_t(samples_to_read / 2),
        2,
        HippoOutputType_s16
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gme_seek_in(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gme_metadata(const char* filename, const HippoServiceAPI* service_api) {
    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);
    gme_info_t* info = nullptr;
    const char* error = nullptr;

    Music_Emu* song = open_gme(filename, io_api);

    if (!song) {
        return -1;
    }

    if ((error = gme_track_info(song, &info, 0)) != nullptr) {
        gme_delete(song);
        hp_info("Unable to get metadata for %s , error %s", filename, error);
        return -1;
    }

    hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    char title_name[1024];
    strcpy(title_name, info->song);

    if (title_name[0] == 0) {
        int filename_start = 0;
        const char* filename_only = find_filename_start(filename, &filename_start);
        strcpy(title_name, filename_only);

        for (int i = (int)strlen(title_name) - 1; i > 0; --i) {
            if (title_name[i] == '.') {
                title_name[i] = 0;
                break;
            }
        }
    }

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title_name);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, info->author);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, info->system);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, info->comment);
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, hippo_max(info->length, 0) / 1000.0f);

    gme_free_info(info);

    int track_count = gme_track_count(song);

    if (track_count > 1) {
        for (int i = 0; i < track_count; ++i) {
            if ((error = gme_track_info(song, &info, i)) == nullptr) {
                char subsong_name[1024] = {0};

                if (info->song[0] != 0) {
                    sprintf(subsong_name, "%s - %s (%d/%d)", title_name, info->song, i + 1, track_count);
                } else {
                    sprintf(subsong_name, "%s (%d/%d)", title_name, i + 1, track_count);
                }

                HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, hippo_max(info->length, 0) / 1000.0f);

                gme_free_info(info);
            }
        }
    }

    gme_delete(song);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void gme_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void gme_static_init(struct HippoLogAPI* log, const struct HippoServiceAPI* service) {
    (void)service;
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin s_gme_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "Game Music Emu",
    "0.0.1",
    "Game Music Emu 0.6.3",
    gme_probe_can_play,
    gme_supported_extensions,
    gme_create,
    gme_destroy,
    gme_event,
    gme_open,
    gme_close_in,
    gme_read_data,
    gme_seek_in,
    gme_metadata,
    gme_static_init,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &s_gme_plugin;
}
