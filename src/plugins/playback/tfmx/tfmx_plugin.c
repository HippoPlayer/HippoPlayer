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
const HippoIoAPI* g_io_api = NULL;

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
    TfmxState state;
    uint16_t temp_data[BUFSIZE];
    void* tune;
    int read_index;
    int frames_decoded;
} TfmxReplayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tfmx_supported_extensions(void* user_data) {
    return "tfmx,tfx,mdat,tfm";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult tfmx_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
    if (strncmp("TFMX-SONG", (char*)data, 9) && strncmp("TFMX_SONG", (char*)data, 9) &&
        strncmp("TFMX-MOD", (char*)data, 8) && strncasecmp("TFMXSONG", (char*)data, 8) &&
        strncasecmp("TFMX ", (char*)data, 5)) {
        hp_debug("Unsupported: %s", filename);
        return HippoProbeResult_Unsupported;
    }

    hp_info("Supported %s", filename);
    return HippoProbeResult_Supported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* tfmx_create(const HippoServiceAPI* service_api) {
    TfmxReplayerData* replayer;

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);
    replayer = (TfmxReplayerData*)malloc(sizeof(struct TfmxReplayerData));
    memset(replayer, 0, sizeof(struct TfmxReplayerData));

    return replayer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_destroy(void* user_data) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int load_two_files(const HippoIoAPI* io_api, TfmxData* mdat, TfmxData* smpl, const char* mdat_name,
                          const char* smpl_name) {
    // first load mdat to memory
    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, mdat_name, (void**)&mdat->data, &mdat->size);

    if (res < 0) {
        hp_warn("Unable to read %s to memory", mdat_name);
        return res;
    }

    // Load sample file to memory
    res = HippoIo_read_file_to_memory(io_api, smpl_name, (void**)&smpl->data, &smpl->size);

    if (res < 0) {
        hp_warn("Unable to read %s to memory", smpl_name);
        HippoIo_free_file_to_memory(io_api, mdat->data);
        return res;
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_load_tfm(const HippoIoAPI* io_api, TfmxState* state, const char* filename) {
    uint8_t* data = NULL;
    uint8_t* smpl_data_ptr = NULL;
    uint64_t size = 0;
    TfmxData mdat = {0};
    TfmxData smpl = {0};

    // first load mdat to memory
    HippoIoErrorCode res = HippoIo_read_file_to_memory(io_api, filename, (void**)&data, &size);

    if (res < 0) {
        hp_warn("Unable to read %s to memory", filename);
        return res;
    }

    // validate that the size is valid
    if (size < sizeof(struct TFMXHeader) + 8 + (3 * 4)) {
        hp_warn("The file is too small %s (can't contain a full tfx file)", filename);
        HippoIo_free_file_to_memory(io_api, data);
        return -1;
    }

    uint8_t* saved_data = data;

    if (strncmp("TFMX-MOD", (char*)data, 8) != 0) {
        hp_warn("The file %s doesn't have correct header data", filename);
        HippoIo_free_file_to_memory(io_api, data);
        return -1;
    }

    data += 8;
    uint32_t smpl_offs = *((uint32_t*)data);
    data += 4;
    uint32_t tag_offs = *((uint32_t*)data);
    data += 8;  // skipping res

    uint32_t mdat_size = smpl_offs - 20 - sizeof(struct TFMXHeader);

    if (mdat_size > 0x10000) {
        smpl_data_ptr = saved_data + smpl_offs;
        mdat_size = 0x10000;
    } else {
        smpl_data_ptr = saved_data + smpl_offs;
        // smpl_data_ptr = saved_data + mdat_size;
    }

    mdat.data = data;
    mdat.size = mdat_size;
    smpl.data = smpl_data_ptr;
    smpl.size = tag_offs - smpl_offs;

    int ret_value = LoadTFMXFile(state, &mdat, &smpl);

    HippoIo_free_file_to_memory(io_api, saved_data);

    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_load(const HippoIoAPI* io_api, const char* url, TfmxState* state) {
    int ret_value = -1;
    int start_offset = 0;
    int ext_offset = 0;
    TfmxData mdat = {0};
    TfmxData smpl = {0};

    const char* filename = find_filename_start(url, &start_offset);
    const char* extension = find_extension(url, &ext_offset);

    // first check if we have mdat.<filename>,smpl.<filename> which is the orignal Amiga naming
    if (strncasecmp(filename, "mdat.", 5) == 0) {
        char* smpl_name = strdup(url);

        // Case-preserving conversion of "mdat" to "smpl"
        smpl_name[start_offset + 0] ^= 'm' ^ 's';
        smpl_name[start_offset + 1] ^= 'd' ^ 'm';
        smpl_name[start_offset + 2] ^= 'a' ^ 'p';
        smpl_name[start_offset + 3] ^= 't' ^ 'l';

        if ((ret_value = load_two_files(io_api, &mdat, &smpl, url, smpl_name) >= 0)) {
            ret_value = LoadTFMXFile(state, &mdat, &smpl);
            HippoIo_free_file_to_memory(io_api, mdat.data);
            HippoIo_free_file_to_memory(io_api, smpl.data);
        }

        free(smpl_name);
        // check if we have <filename>.mdat., <filename>.smpl
    } else if (strncasecmp(extension, ".mdat", 5) == 0) {
        char* smpl_name = strdup(url);

        // Case-preserving conversion of "mdat" to "smpl"
        smpl_name[ext_offset + 0] ^= 'm' ^ 's';
        smpl_name[ext_offset + 1] ^= 'd' ^ 'm';
        smpl_name[ext_offset + 2] ^= 'a' ^ 'p';
        smpl_name[ext_offset + 3] ^= 't' ^ 'l';

        if ((ret_value = load_two_files(io_api, &mdat, &smpl, url, smpl_name) >= 0)) {
            ret_value = LoadTFMXFile(state, &mdat, &smpl);
            HippoIo_free_file_to_memory(io_api, mdat.data);
            HippoIo_free_file_to_memory(io_api, smpl.data);
        }

        free(smpl_name);
        // Check for <filename>.tfx, <filename>.sam
    } else if (strncasecmp(extension, ".tfx", 4) == 0) {
        char* smpl_name = strdup(url);

        // Case-preserving conversion of "tfx" to "samp"
        smpl_name[ext_offset + 0] ^= 't' ^ 's';
        smpl_name[ext_offset + 1] ^= 'f' ^ 'a';
        smpl_name[ext_offset + 2] ^= 'x' ^ 'm';

        if ((ret_value = load_two_files(io_api, &mdat, &smpl, url, smpl_name) >= 0)) {
            ret_value = LoadTFMXFile(state, &mdat, &smpl);
            HippoIo_free_file_to_memory(io_api, mdat.data);
            HippoIo_free_file_to_memory(io_api, smpl.data);
        }

        free(smpl_name);
        // Check for .tfm which is a single format
    } else if (strncasecmp(extension, ".tfm", 3) == 0) {
        ret_value = tfmx_load_tfm(io_api, state, url);
    }

    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_open(void* user_data, const char* buffer, int subsong) {
    TfmxReplayerData* plugin = (TfmxReplayerData*)user_data;
    (void)plugin;

    TfmxState_init(&plugin->state);

    if (tfmx_load(g_io_api, buffer, &plugin->state) < 0) {
        hp_error("Unable to open %s", buffer);
        return -1;
    }

    TFMXSetSubSong(&plugin->state, subsong);
    TFMXRewind(&plugin->state);

    hp_info("Starting to play %s (subsong %d)", buffer, subsong);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_close(void* user_data) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo tfmx_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct TfmxReplayerData* data = (TfmxReplayerData*)user_data;

    int block_size = (int)tfmx_get_block_size(&data->state) / 2;

    if (tfmx_try_to_make_block(&data->state) >= 0) {
        tfmx_get_block(&data->state, dest);
    }

    HippoReadInfo t = {
        48000,
        block_size / 2,
        2,
        HippoOutputType_s16
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_metadata(const char* url, const HippoServiceAPI* service_api) {
    TfmxState* state = malloc(sizeof(TfmxState));
    TfmxState_init(state);

    const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    if (tfmx_load(io_api, url, state) < 0) {
        hp_error("Unable to get metadata for %s", url);
        free(state);
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
    tfmx_fill_text_info(state, text_info);
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, text_info);

    int subsongs_count = TFMXGetSubSongs(state);

    if (subsongs_count > 1) {
        for (int i = 0; i < subsongs_count; ++i) {
            char subsong_name[1024] = {0};
            sprintf(subsong_name, "%s (%d/%d)", title, i + 1, subsongs_count);
            HippoMetadata_add_subsong(metadata_api, index, i, subsong_name, 0.0f);
        }
    }

    free(state);

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
    "1.0.2",
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
