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
	LinearAllocator* allocator = LinearAllocator_getScratchPad();
	LinearAllocatorRewindPoint rewind = LinearAllocator_getRewindPoint(allocator);

	// TODO: Scan for plugins

	static const char* plugins[] = 
	{
		"../../src/tundra-output/macosx-clang-debug-default/libHivelyPlugin.dylib",
		"../../src/tundra-output/macosx-clang-debug-default/libFlacPlugin.dylib",
		"../../src/tundra-output/macosx-clang-debug-default/libMikmodPlugin.dylib",
		"../../src/tundra-output/macosx-clang-debug-default/libSidPlugin.dylib",
		"../../src/tundra-output/macosx-clang-debug-default/libFutureComposerPlugin.dylib",
		0,
	};

	HippoPlugins_add(plugins);
	LinearAllocator_rewind(allocator, rewind);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoMain_create()
{
	HippoWindowRect rect;

 	struct lua_State* luaState; 
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


