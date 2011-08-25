#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "audio/HippoAudio.h"
#include "graphics/HippoWindow.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"
#include "plugin_api/HippoPlugin.h"
#include "audio/HippoAudio.h"
#include <math.h>

#if defined(HIPPO_WIN32)
#include <windows.h>
#endif

extern void HippoGui_drawClassic();
static HippoAudioDevice g_audioDevices[64];
extern HippoPlaybackPlugin* HivelyPlugin_getPlugin();

struct MyData
{
	float sinWave;
};

/*

static int readData(void* userData, void* dest, int size)
{
	struct MyData* data = (struct MyData*)userData;
	float* destOut = (float*)dest;

	for (int i = 0; i < size; ++i)
	{
		data->sinWave += 0.01f;
		*destOut++ = (float)sin(data->sinWave);
	}

	return size;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(HIPPO_WIN32)
extern HINSTANCE g_winInstance; 

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HippoWindowRect rect;
	struct HippoWindow* window;
	g_winInstance = hInstance;
#else
int main()
{
	HippoWindowRect rect;
	struct HippoWindow* window;
#endif

	int deviceCount = HippoAudio_buildDeviceList(g_audioDevices, 64);

	for (int i = 0; i < deviceCount; ++i)
	{
		printf("name %s id %p\n", g_audioDevices[i].name, g_audioDevices[i].deviceId);
	}

	struct MyData data;
	data.sinWave = 0.0f;

	//HippoPlaybackPlugin plugin;
	//plugin.userData = &data;
	//plugin.readData = readData;

	HippoPlaybackPlugin* plugin = HivelyPlugin_getPlugin();
	plugin->create(plugin->userData);
	plugin->open(plugin->userData, "songs/ahx/geir_tjelta_-_a_new_beginning.ahx");

	HippoAudio_openDefaultOutput(plugin);

	/*
	HippoImage image;

	if (!HippoImageLoader_loadFile(&image, "test.png"))
	{
		printf("Failed to open %s\n", "test.png");
	}
	else
	{
		printf("0x%p %d %d %d\n", image.data, image.width, image.height, image.comp);
	}
	*/

	rect.x = 50;
	rect.y = 50;
	rect.width = 250;
	rect.height = 250;

	window = HippoWindow_create(0, "foo", 0, &rect);
	HippoGui_reset();

	while (!HippoWindow_isClosed())
	{
		HippoGui_drawClassic();
		HippoWindow_updateEvents();
	}

	HippoAudio_close();

	return 0;
}
