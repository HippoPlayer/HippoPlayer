#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/memory/LinearAllocator.h"
#include "core/HippoLua.h"
#include "core/io/HippoSharedObject.h"
#include "core/io/HippoIo.h"
#include "audio/HippoAudio.h"
#include "graphics/HippoWindow.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"
#include "plugin_api/HippoPlugin.h"
#include "audio/HippoAudio.h"
#include "HippoPlugins.h"
#include <math.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void initPlugins()
{
#if defined(HIPPO_MACOSX)
	//const char* libExtension = "dylib";
#elif defined(HIPPO_WIN32)
	const char* libExtension = "dll";
#else
#error "Unsupported platform
#endif

	LinearAllocator* allocator = LinearAllocator_getScratchPad();
	LinearAllocatorRewindPoint rewind = LinearAllocator_getRewindPoint(allocator);

	// temp

	static const char* plugins[] = 
	{
		"../../src/tundra-output/macosx-gcc-debug-default/libHivelyPlugin.dylib",
		0,
	};

	// Find all the plugins (temporary in the current directory of the executable)
	//const char** pluginFiles = HippoIo_scanDirectory(allocator, "../../src/tundra-output/macosx-gcc-debug-default", libExtension, false);
	HippoPlugins_add(plugins);
	
	/*
	while (*pluginFiles)
	{
		printf("found plugin %s\n", *pluginFiles);
		pluginFiles++;
	}
	*/

	LinearAllocator_rewind(allocator, rewind);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoMain_create()
{
	struct lua_State* luaState; 
	HippoWindowRect rect;
	struct HippoWindow* window;
	uint32_t scratchSize = 2 * 1024 * 1024;  // 2 meg of scratchmemory

	// TODO: Us mmap/VirtualAlloc instead of malloc
	LinearAllocator_setScratchPad(malloc(scratchSize), scratchSize);

	initPlugins();

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
	plugin->create(plugin->userData);
	plugin->open(plugin->userData, "songs/ahx/geir_tjelta_-_a_new_beginning.ahx");
	*/

	HippoAudio_openDefaultOutput();

	// call the creation code

	lua_getglobal(luaState, "create");
	lua_pcall(luaState, 0, 0, 0);

	rect.x = 50;
	rect.y = 50;
	rect.width = 250;
	rect.height = 250;

	window = HippoWindow_create(0, "foo", 0, &rect);
	HippoGui_reset();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoMain_destroy()
{
	lua_getglobal(g_luaState, "destroy");
	lua_pcall(g_luaState, 0, 0, 0);
}


