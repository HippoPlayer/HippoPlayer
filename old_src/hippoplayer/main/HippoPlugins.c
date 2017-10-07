#include "HippoPlugins.h"
#include "core/io/HippoSharedObject.h"
#include "plugin_api/HippoPlugin.h"
#include "audio/HippoAudio.h"
#include <stdio.h>
#include <string.h>

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
// TODO: Move to string util class

static const char* getFileExt(const char* file)
{
	int length = (int)strlen(file);

	for (int i = length - 1; i > 0; --i)
	{
		if (file[i] == '.')
			return &file[i + 1];
	}

	// unable to find extension

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoPlugins_playFile(const char* file)
{
	// Assume the first plugin will play the correct file for now

	const char* fileExtension = getFileExt(file);

	printf("%s\n", fileExtension);

	for (uint32_t i = 0, pluginCount = g_pluginCount; i < pluginCount; ++i)
	{
		HippoPlaybackPlugin* plugin = (HippoPlaybackPlugin*)g_plugins[i];

		// TODO: We might have several plugins supporting same time so we need the user to have the abilitty to select
		// the default one

		const char** supportedTypes = plugin->supportedExtensions(plugin->privateData);
		if (!supportedTypes)
		{
			printf("No supported file types registered for plugin, skipping\n");
			continue;
		}

		while (*supportedTypes)
		{
			if (!strcmp(*supportedTypes, fileExtension))
			{
				plugin->open(plugin->privateData, file);
				HippoAudio_preparePlayback(plugin);
				return;
			}

			supportedTypes++;
		}

	}


	printf("lets play!\n");
}

