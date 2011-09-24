#include "HippoPlugins.h"
#include "core/io/HippoSharedObject.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	HippoMaxPlugins = 2048,
};

static void* g_plugins[HippoMaxPlugins];
//static void* g_plugins[HippoMaxPlugins];

static uint32_t g_pluginCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoPlugins_add(const char** plugins)
{
	const char* filename = *plugins;

	while (filename)
	{
		void* function;
		HippoHandle handle;
		
		handle = HippoSharedObject_open(filename);

		if (!handle)
		{
			printf("Unable to open %s\n", filename); 
			goto next;
		}

		function = HippoSharedObject_getSym(handle, "getPlugin");

		if (!function)
		{
			printf("Unable to find getPlugin\n");
			goto next;
		}

next:;
		filename = *++plugins;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

