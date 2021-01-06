#include <HippoPlugin.h>
#include "dr_flac.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE 1024

static const struct HippoIoAPI* g_io_api = 0;
HippoLogAPI* g_hp_log = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ReplayerData {
	drflac* song;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* flac_supported_extensions() {
    return "flac";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* flac_create(const HippoServiceAPI* service_api) {
    void* data = malloc(sizeof(struct ReplayerData));
    memset(data, 0, sizeof(struct ReplayerData));

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flac_destroy(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    if (data->song) {
        drflac_free(data->song, NULL);
    }

    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flac_open(void* user_data, const char* filename, int subsong, const struct HippoSettingsAPI* api) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    (void)api;

	// TODO: Use IO APIs
	data->song = drflac_open_file(filename, NULL);

    if (!data->song) {
        hp_error("Unable to open: %s", filename);
        return -1;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flac_close(void* user_data) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;
	drflac_free(data->song, NULL);
    data->song = NULL;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult flac_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename,
                                          uint64_t total_size) {
	// TODO: Implement proper reader here
	drflac* song = drflac_open_file(filename, NULL);

    if (song) {
		drflac_free(song, NULL);
        hp_info("Supported: %s", filename);
        return HippoProbeResult_Supported;
    }

    hp_debug("Unsupported: %s", filename);
    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo flac_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                    uint32_t native_sample_rate) {
    struct ReplayerData* data = (struct ReplayerData*)user_data;

    uint16_t samples_to_read = hippo_min(max_output_bytes / 8, FRAME_SIZE);

	drflac_read_pcm_frames_s32(data->song, samples_to_read, (drflac_int32*)dest);

    HippoReadInfo t = {
    	data->song->sampleRate,
        samples_to_read,
        data->song->channels,
        HippoOutputType_s32,
    };

    return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flac_seek(void* user_data, int ms) {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct IdMap {
	const char* flac_id;
	const char* metadata_id;
} IdMap;

#define sizeof_array(t) (sizeof(t) / sizeof(t[0]))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static IdMap s_ids[] = {
	{ "TITLE", HippoMetadata_TitleTag },
	{ "ALBUM", HippoMetadata_AlbumTag },
	{ "ARTIST", HippoMetadata_ArtistTag },
	{ "COMMENT", HippoMetadata_MessageTag },
	{ "GENRE", HippoMetadata_GenreTag },
	{ "DATE", HippoMetadata_DateTag },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct MetadataCallback {
	const HippoMetadataAPI* api;
	HippoMetadataId index;
} MetadataCallback;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void flac_metadata_callback(void* userdata, drflac_metadata* metadata) {
    const MetadataCallback* data = (const MetadataCallback*)userdata;

	const char* comment = NULL;
	drflac_uint32 comment_out_len = 0;
	drflac_vorbis_comment_iterator iter;

	if (metadata->type != DRFLAC_METADATA_BLOCK_TYPE_VORBIS_COMMENT) {
		return;
	}

	drflac_init_vorbis_comment_iterator(&iter,
		metadata->data.vorbis_comment.commentCount,
		metadata->data.vorbis_comment.pComments);

	while ((comment = drflac_next_vorbis_comment(&iter, &comment_out_len))) {
		char temp[2048] = { 0 };
		strncpy(temp, comment, comment_out_len);

		char* pch = strchr(temp, '=');

		if (!pch) {
			hp_warn("FLAC metadata comment is invalid", temp);
			break;
		}

		*pch = 0;

		for (int i = 0; i < sizeof_array(s_ids); ++i) {
			if (!strcmp(temp, s_ids[i].flac_id)) {
				HippoMetadata_set_tag(data->api, data->index, s_ids[i].metadata_id, pch + 1);
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flac_metadata(const char* filename, const HippoServiceAPI* service_api) {
    //const HippoIoAPI* io_api = HippoServiceAPI_get_io_api(service_api, HIPPO_FILE_API_VERSION);
    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);

    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

	MetadataCallback callback_data = {
		metadata_api,
		index,
	};

	// TODO: Use reading APIs
	drflac* song = drflac_open_file_with_metadata(filename, flac_metadata_callback, &callback_data, NULL);

    if (!song) {
        hp_error("Unable to init %s", filename);
        return -1;
    }

    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "FLAC");

	HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag,
		song->totalPCMFrameCount / (float)song->sampleRate);

    drflac_free(song, NULL);

    printf("ok\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void flac_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void flac_static_init(struct HippoLogAPI* log, const HippoServiceAPI* service_api) {
	(void)service_api;
    g_hp_log = log;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin s_flac_plugin = {
    HIPPO_PLAYBACK_PLUGIN_API_VERSION,
    "flac",
    "0.0.1",
    "dr_flac 0.12.24",
    flac_probe_can_play,
    flac_supported_extensions,
    flac_create,
    flac_destroy,
    flac_event,
    flac_open,
    flac_close,
    flac_read_data,
    flac_seek,
    flac_metadata,
    flac_static_init,
    NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
    return &s_flac_plugin;
}


