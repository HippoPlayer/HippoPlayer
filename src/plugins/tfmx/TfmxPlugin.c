#include "../../plugin_api/HippoPlugin.h"
#include "src/tfmx.h"
#include "src/tfmx_iface.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* getFileNameFromPath(const char* path)
{
   for(size_t i = strlen(path) - 1;  i > 0; i--)
   {
      if (path[i] == '/')
      {
         return &path[i+1];
      }
   }

   return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct TfmxReplayerData
{
	void* tune;
	const char* name;

} TfmxReplayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tfmx_info(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tfmx_track_info(void* user_data) {
	TfmxReplayerData* plugin = (TfmxReplayerData*)user_data;

	if (!plugin || !plugin->name) {
		return "TFMX: <unknown>";
	}

	return plugin->name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tfmx_supported_extensions(void* user_data) {
	return "tfmx,TFX";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: These checks needs to be made much better (sanity check some more sizes in the pattern etc)

enum HippoProbeResult tfmx_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
    if (strncmp("TFMX-SONG", (char*)data, 9)
    	&& strncmp("TFMX_SONG", (char*)data, 9)
		&& strncasecmp("TFMXSONG", (char*)data, 8)
		&& strncasecmp("TFMX ", (char*)data, 5)) {

    	return HippoProbeResult_Unsupported;
	}

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

static int tfmx_open(void* user_data, const char* buffer) {
	TfmxReplayerData* plugin = (TfmxReplayerData*)user_data;

	if (LoadTFMXFile((char*)buffer) != 0)
	    return -1;

	TFMXSetSubSong(0);
    TFMXRewind();

    plugin->name = strdup(getFileNameFromPath(buffer));

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_close(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_read_data(void* user_data, void* dest) {
	int16_t temp_data[BUFSIZE] = { 0 };

    int block_size = (int)tfmx_get_block_size() / 2;

    assert(block_size < BUFSIZE);

    if (tfmx_try_to_make_block() >= 0) {
	    tfmx_get_block(temp_data);
    } else {
        memset(temp_data, 0, block_size);
    }

	const float scale = 1.0f / 32768.0f;

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

static int tfmx_frame_size(void* user_data) {
    return tfmx_get_block_size() / 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int tfmx_length(void* user_data) {
	return -10;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_tfmx_plugin = {
	1,
	tfmx_probe_can_play,
	tfmx_info,
	tfmx_track_info,
	tfmx_supported_extensions,
	tfmx_create,
	tfmx_destroy,
	tfmx_open,
	tfmx_close,
	tfmx_read_data,
	tfmx_seek,
	tfmx_frame_size,
	tfmx_length,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* getPlugin() {
	return &g_tfmx_plugin;
}


