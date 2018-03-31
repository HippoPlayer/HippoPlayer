#include "../../plugin_api/HippoPlugin.h"
#include "src/mdxmini.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _WIN32
#include <libgen.h>
#else
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

char* sj2utf8(const char* input);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a mdx_plugin plugin used as a reference and not inteded to do anything

typedef struct MDXPlugin {
	char title[4096];
	t_mdxmini mdx_tune;
} MDXPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* mdx_plugin_supported_extensions() {
	return "mdx";
}

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

/*
static const char* mdx_plugin_track_info(void* user_data) {
	MDXPlugin* plugin = (MDXPlugin*)user_data;
	return plugin->title;

	// Not working correct so using filename for now

	//mdx_get_title(&plugin->mdx_tune, temp_title);
	//return sj2utf8(temp_title);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I haven't found any way to probe the MDX file so have to do with extension compare :/

enum HippoProbeResult mdx_plugin_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	const char* point = 0;

	if ((point = strrchr(filename,'.')) != NULL) {
		if (strcasecmp(point,".mdx") == 0) {
			return HippoProbeResult_Supported;
		}
	}

	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* mdx_plugin_create(struct HippoServiceAPI* services) {
	MDXPlugin* plugin = malloc(sizeof(MDXPlugin));
	memset(plugin, 0, sizeof(MDXPlugin));

	mdx_set_rate(48000);

	return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char* get_dirname(const char* src, char* dir_output)
{
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

static int mdx_plugin_open(void* user_data, const char* buffer) {
	char dir_name[4096];
	MDXPlugin* plugin = (MDXPlugin*)user_data;

	get_dirname(buffer, dir_name);

	strcpy(plugin->title, getFileNameFromPath(buffer));

	if (mdx_open(&plugin->mdx_tune, (char*)buffer, dir_name) >= 0) {
		return 1;
	}

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

static int mdx_plugin_read_data(void* user_data, void* dest, uint32_t max_samples) {
	MDXPlugin* plugin = (MDXPlugin*)user_data;
	(void)plugin;

	short data[480 * 2] = { 0 };

	mdx_calc_sample(&plugin->mdx_tune, data, (480 * 2) / 2);

	float* new_dest = (float*)dest;

	const float scale = 1.0f / 32767.0f;

	for (int i = 0; i < 480 * 2; ++i) {
		new_dest[i] = ((float)data[i]) * scale;
	}

	return 480 * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int mdx_plugin_plugin_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int mdx_plugin_length(void* user_data) {
	MDXPlugin* plugin = (MDXPlugin*)user_data;
	(void)plugin;

	int len = mdx_get_length(&plugin->mdx_tune);

	if (len > 0) {
		return len;
	}

	return -10;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_mdx_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	mdx_plugin_probe_can_play,
	mdx_plugin_supported_extensions,
	mdx_plugin_create,
	mdx_plugin_destroy,
	mdx_plugin_open,
	mdx_plugin_close,
	mdx_plugin_read_data,
	mdx_plugin_plugin_seek,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_mdx_plugin;
}





