#include <HippoPlugin.h>
#include <asap.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREQ 44100
#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* find_extension(const char* path, int* offset) {
    for (size_t i = strlen(path) - 1; i > 0; i--) {
        if (path[i] == '.') {
            *offset = (int)(i + 1);
            return &path[i];
        }
    }

    *offset = 0;
    return path;
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

struct ReplayerData {
    ASAP* song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* asap_supported_extensions() {
    return "ctc";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* asap_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int asap_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if (data->song) {
        ASAP_Delete(data->song);
    }

    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ASAP* open_asap(const char* filename, const struct HippoIoAPI* io) {
    uint64_t size = 0;
    void* data;

    HippoIoErrorCode res = HippoIo_read_file_to_memory(io, filename, &data, &size);

    if (res < 0) {
        hp_error("Unable to open: %s", filename);
        return NULL;
    }

    ASAP* song = ASAP_New();

    if (!ASAP_Load(song, filename, (uint8_t*)data, (int)size)) {
        ASAP_Delete(song);
        song = NULL;
    }

    HippoIo_free_file_to_memory(io, data);

    return song;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int asap_open(void* user_data, const char* filename, int subsong) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if (!(data->song = open_asap(filename, g_io_api))) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    ASAP_PlaySong(data->song, subsong, -1);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int asap_close(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
    ASAP_Delete(data->song);
    data->song = NULL;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult asap_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    ASAP* song = ASAP_New();

    if (ASAP_Load(song, filename, (uint8_t*)data, (int)data_size)) {
        ASAP_Delete(song);
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo asap_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

    int gen_samples = ASAP_Generate(data->song, (uint8_t*)dest, samples_to_read, ASAPSampleFormat_S16_L_E) / 2;

    HippoReadInfo t = {
        ASAP_SAMPLE_RATE,
        gen_samples,
        ASAPInfo_GetChannels(ASAP_GetInfo(data->song)),
        HippoOutputType_s16
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int asap_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int asap_metadata(const char* filename, const HippoServiceAPI* service_api) {
    int ext_offset = 0;
    int filename_start = 0;
    char title_name[1024];

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    ASAP* song = open_asap(filename, io_api);

    if (!song) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

    const ASAPInfo* info = ASAP_GetInfo(song);

    hp_info("Updating metadata for %s", filename);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    const char* title = ASAPInfo_GetTitle(info);
    const char* extension = find_extension(filename, &ext_offset);

    if (title == NULL || title[0] == 0) {
        const char* filename_only = find_filename_start(filename, &filename_start);
        strcpy(title_name, filename_only);

        for (int i = 0; i < strlen(title_name); ++i) {
            if (title_name[i] == '.') {
                title_name[i] = 0;
                break;
            }
        }

        title = title_name;
    }

    if (ext_offset != 0) {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag,
                              ASAPInfo_GetExtDescription(extension + 1));
    } else {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "ASAP");
    }

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, title);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, ASAPInfo_GetAuthor(info));

    int dur = ASAPInfo_GetDuration(info, 0);

    if (dur != -1) {
        HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, dur / 1000.f);
    } else {
        HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, 0.0f);
    }

    int songs = ASAPInfo_GetSongs(info);

    if (songs > 1) {
        for (int i = 0; i < songs; ++i) {
            float duration = ASAPInfo_GetDuration(info, i) / 1000.0f;
            char subsong_name[1024] = {0};
            sprintf(subsong_name, "%s (%d/%d)", title, i + 1, songs);

            if (duration > 0.0f) {
                HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, duration);
            } else {
                HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
            }
        }
    }

    ASAP_Delete(song);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void asap_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void asap_set_log(struct HippoLogAPI* log) {
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_asap_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "asap",
    "0.0.1",
    "ASAP " ASAPInfo_VERSION,
    asap_probe_can_play,
    asap_supported_extensions,
    asap_create,
    asap_destroy,
    asap_event,
    asap_open,
    asap_close,
    asap_read_data,
    asap_seek,
    asap_metadata,
    asap_set_log,
    NULL,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &g_asap_plugin;
}
