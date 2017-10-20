#include <libopenmpt/libopenmpt.hpp>
#include "../../plugin_api/HippoPlugin.h"

#include <vector>
#include <string>
#include <string.h>

const int MAX_EXT_COUNT = 16 * 1024;
static char s_supported_extensions[MAX_EXT_COUNT];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct OpenMptData {
    openmpt::module* mod = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* openmpt_info(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* openmpt_track_info(void* userData) {
	return 0;
}

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

static void* openmpt_create() {
	void* replayerData = malloc(sizeof(struct OpenMptData));
	memset(replayerData, 0, sizeof(struct OpenMptData));

	return replayerData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_destroy(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_open(void* user_data, const char* buffer) {
	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc
	FILE* file = fopen(buffer, "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8_t* data = new uint8_t[size];
	fread(data, size, 1, file);
	fclose(file);

	struct OpenMptData* replayerData = (struct OpenMptData*)user_data;
    replayerData->mod = new openmpt::module(data, size);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_close(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_frame_size(void* user_data) {
	return 480 * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_read_data(void* user_data, void* dest) {
	struct OpenMptData* replayerData = (struct OpenMptData*)user_data;

	// count is number of frames per channel and div by 2 as we have 2 channels
	const int count = openmpt_frame_size(user_data) / 2;
    return replayerData->mod->read_interleaved_stereo(48000, count, (float*)dest) * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int openmpt_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_openmptPlugin = {
	1,
	openmpt_info,
	openmpt_track_info,
	openmpt_supported_extensions,
	openmpt_create,
	openmpt_destroy,
	openmpt_open,
	openmpt_close,
	openmpt_read_data,
	openmpt_seek,
	openmpt_frame_size,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* getPlugin() {
	return &g_openmptPlugin;
}


