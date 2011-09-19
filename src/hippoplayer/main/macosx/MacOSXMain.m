#import <Cocoa/Cocoa.h>
#include "core/file/HippoSharedObject.h"
#include "core/HippoLua.h"
#include "audio/HippoAudio.h"
#include "graphics/HippoWindow.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"
#include "plugin_api/HippoPlugin.h"
#include "audio/HippoAudio.h"
#include <math.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	struct lua_State* luaState; 
	HippoWindowRect rect;
	struct HippoWindow* window;

	[NSApplication sharedApplication];
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	luaState = g_luaState = luaL_newstate();

	luaL_openlibs(luaState);
	HippoGui_registerLuaFunctions(luaState);
	HippoLua_registerLuaFunctions(luaState);

	if (luaL_loadfile(luaState, "skins/classic/ui.lua") || lua_pcall(luaState, 0, 0, 0) != 0)
	{
		printf("Failed to load skins/classic/ui.lua\n");
		return 0;
	}

	/*
	int deviceCount = HippoAudio_buildDeviceList(g_audioDevices, 64);

	for (int i = 0; i < deviceCount; ++i)
	{
		printf("name %s id %p\n", g_audioDevices[i].name, g_audioDevices[i].deviceId);
	}
	*/

	/*
	HippoHandle handle = HippoSharedObject_open("../../src/tundra-output/macosx-gcc-debug-default/libHivelyPlugin.dylib");

	if(!handle)
	{
		printf("Unable to open sharedlib\n");
		return 0;
	}
	
	void* function = HippoSharedObject_getSym(handle, "getPlugin");

	if(!function)
	{
		printf("Unable to find getPlugin\n");
		return 0;
	}

	void* (*funcPtr)();
	*(void **)(&funcPtr) = function;
	HippoPlaybackPlugin* plugin = (HippoPlaybackPlugin*)funcPtr();

	plugin->create(plugin->userData);
	plugin->open(plugin->userData, "songs/ahx/geir_tjelta_-_a_new_beginning.ahx");
	HippoAudio_openDefaultOutput(plugin);
	*/

	// call the creation code

	lua_getglobal(luaState, "create");
	lua_pcall(luaState, 0, 0, 0);

	rect.x = 50;
	rect.y = 50;
	rect.width = 250;
	rect.height = 250;

	window = HippoWindow_create(0, "foo", 0, &rect);
	HippoGui_reset();

	HippoGui_begin();
	lua_getglobal(luaState, "update");
	lua_call(luaState, 0, 0);
	HippoGui_end();

	[NSApp run];

	[pool drain]; 

	printf("main exit\n");

	return 0;
}
		

