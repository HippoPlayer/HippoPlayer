#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/file/HippoSharedObject.h"
#include "core/HippoLua.h"
#include "audio/HippoAudio.h"
#include "graphics/HippoWindow.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"
#include "plugin_api/HippoPlugin.h"
#include <math.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void HippoGui_drawClassic();
extern HINSTANCE g_winInstance; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	struct lua_State* luaState; 
	HippoWindowRect rect;
	struct HippoWindow* window;
	g_winInstance = hInstance;

	AllocConsole();
	freopen("CONOUT$","wb",stdout);

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
	rect.width = 264;
	rect.height = 136;

	window = HippoWindow_create(0, "foo", 0, &rect);
	HippoGui_reset();

	while (!HippoWindow_isClosed())
	{
		HippoWindow_updateEvents();
	}

	lua_getglobal(luaState, "destroy");
	lua_pcall(luaState, 0, 0, 0);

	return 0;
}

