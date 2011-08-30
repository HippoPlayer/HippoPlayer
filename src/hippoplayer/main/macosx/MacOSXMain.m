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

	luaState = g_luaState = luaL_newstate();

	luaL_openlibs(luaState);
	HippoGui_registerLuaFunctions(luaState);
	HippoLua_registerLuaFunctions(luaState);

	if (luaL_loadfile(luaState, "skins/classic/ui.lua") || lua_pcall(luaState, 0, 0, 0) != 0)
	{
		printf("Failed to load skins/classic/ui.lua\n");
		return 0;
	}

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

	printf("main exit\n");

	return 0;
}
		

