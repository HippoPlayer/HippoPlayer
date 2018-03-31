#include <libopenmpt/libopenmpt.hpp>
#include "../../plugin_api/HippoPlugin.h"

#include <vector>
#include <string>
#include <string.h>
#include <assert.h>

const int MAX_EXT_COUNT = 16 * 1024;
static char s_supported_extensions[MAX_EXT_COUNT];
static HippoIoAPI* g_io_api = nullptr;
static HippoMetadataAPI* g_metadata_api = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct OpenMptData {
    openmpt::module* mod = 0;
    std::string song_title;
    void* song_data = 0;
    float length;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static const char* openmpt_track_info(void* userData) {
    OpenMptData* data = (OpenMptData*)userData;
	return data->song_title.c_str();
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* openmpt_supported_extensions() {
	// If we have already populated this we can just return

	if (s_supported_extensions[0] != 0) {
		return s_supported_extensions;
	}

	std::vector<std::string> ext_list = openmpt::get_supported_extensions();
	size_t count = ext_list.size();

	for (size_t i = 0; i < count; ++i) {
		//strcat_s(s_supported_extensions, MAX_EXT_COUNT, ext_list[i].c_str());
		strcat(s_supported_extensions, ext_list[i].c_str());
		if (i != count - 1) {
			strcat(s_supported_extensions, ",");
		}
	}

	return s_supported_extensions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* openmpt_create(HippoServiceAPI* service_api) {
    OpenMptData* user_data = new OpenMptData;

    g_io_api = HippoServiceAPI_get_io_api(service_api, 1);
    g_metadata_api = HippoServiceAPI_get_metadata_api(service_api, 1);

	return (void*)user_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_destroy(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoProbeResult openmpt_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    int res = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, data, data_size, total_size);

    switch (res) {
        case openmpt::probe_file_header_result_success :
        {
            return HippoProbeResult_Supported;
        }
        case openmpt::probe_file_header_result_failure :
        {
            return HippoProbeResult_Unsupported;
        }

        case openmpt::probe_file_header_result_wantmoredata :
        {
            printf("openmpt: Unable to probe because not enough data\n");
            break;
        }
    }

    printf("openmpt: case %d not handled in switch. Assuming unsupported file\n", res);

    return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_open(void* user_data, const char* filename) {
	char keyname[32];
    uint64_t size = 0;
	struct OpenMptData* replayer_data = (struct OpenMptData*)user_data;

    HippoIoErrorCode res = g_io_api->read_file_to_memory(g_io_api->priv_data,
        filename, &replayer_data->song_data, &size);

    if (res < 0) {
        return -1;
    }

    replayer_data->mod = new openmpt::module(replayer_data->song_data, size);
    replayer_data->song_title = replayer_data->mod->get_metadata("title");
    replayer_data->length = replayer_data->mod->get_duration_seconds();

    const auto& mod = replayer_data->mod;

	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("title").c_str(), "title");
	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("type_long").c_str(), "type");
	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("tracker").c_str(), "authoring_tool");
	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("artist").c_str(), "artist");
	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("date").c_str(), "date");
	HippoMetadata_set_key(g_metadata_api, filename, 0, mod->get_metadata("message_raw").c_str(), "message");

	int i = 1;

	for (const auto& sample : mod->get_sample_names()) {
		sprintf(keyname, "sample_%04d", i++);
		HippoMetadata_set_key(g_metadata_api, filename, 0, sample.c_str(), keyname);
	}

	i = 1;

	for (const auto& instrument : mod->get_instrument_names()) {
		sprintf(keyname, "instrument_%04d", i++);
		HippoMetadata_set_key(g_metadata_api, filename, 0, instrument.c_str(), keyname);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_close(void* user_data) {
	struct OpenMptData* replayer_data = (struct OpenMptData*)user_data;

	if (g_io_api) {
	    g_io_api->free_file_to_memory(g_io_api->priv_data, replayer_data->song_data);
	}

	delete replayer_data->mod;
	delete replayer_data;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_read_data(void* user_data, void* dest, uint32_t max_samples) {
	struct OpenMptData* replayerData = (struct OpenMptData*)user_data;

	// count is number of frames per channel and div by 2 as we have 2 channels
	const int count = 480;
    return replayerData->mod->read_interleaved_stereo(48000, count, (float*)dest) * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int openmpt_length(void* user_data) {
    OpenMptData* data = (OpenMptData*)user_data;
	return int(data->length);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_openmptPlugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	openmpt_probe_can_play,
	openmpt_supported_extensions,
	openmpt_create,
	openmpt_destroy,
	openmpt_open,
	openmpt_close,
	openmpt_read_data,
	openmpt_seek,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_openmptPlugin;
}


