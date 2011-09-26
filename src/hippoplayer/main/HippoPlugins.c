#include "HippoPlugins.h"
#include "core/io/HippoSharedObject.h"
#include "plugin_api/HippoPlugin.h"
#include "audio/HippoAudio.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	HippoMaxPlugins = 2048,
};

static void* g_plugins[HippoMaxPlugins];
static uint32_t g_pluginCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoPlugins_add(const char** plugins)
{
	const char* filename = *plugins;
	uint32_t pluginCount = g_pluginCount;

	while (filename)
	{
		void* (*funcPtr)();
		void* function;
		HippoHandle handle;
		HippoPlaybackPlugin* plugin;

		// TODO: Support more than playback plugins

		handle = HippoSharedObject_open(filename);

		if (!handle)
		{
			printf("Unable to open %s\n", filename); 
			goto next;
		}

		function = HippoSharedObject_getSym(handle, "getPlugin");

		if (!function)
		{
			printf("Unable to find getPlugin for %s\n", filename);
			goto next;
		}

		*(void **)(&funcPtr) = function;
		plugin = (HippoPlaybackPlugin*)funcPtr();

		printf("Added plugin %s\n", filename);

		// Initialize the plugin 
		plugin->privateData = plugin->create();
		g_plugins[pluginCount++] = plugin;
next:;
		filename = *++plugins;
	}

	g_pluginCount = pluginCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoPlugins_playFile(const char* file)
{
	// Assume the first plugin will play the correct file for now

	printf("lets play!\n");
	HippoPlaybackPlugin* plugin = (HippoPlaybackPlugin*)g_plugins[0];
	plugin->open(plugin->privateData, file);
	HippoAudio_preparePlayback(plugin);
}

