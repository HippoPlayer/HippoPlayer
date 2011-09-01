#include "HippoLua.h"
#include "core/file/HippoFile.h"
#include "graphics/gui/HippoGui.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

lua_State* g_luaState = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoLua_updateScript()
{
	HippoGui_begin();
	lua_getglobal(g_luaState, "update");
	lua_pcall(g_luaState, 0, 0, 0);
	HippoGui_end();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Hippo_quit();

static int luaQuit(lua_State* luaState)
{
	Hippo_quit();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int luaOpenFileDialog(lua_State* luaState)
{
	char filename[1024];

	int ret = HippoFile_openDialog(filename, sizeof(filename));

	if (ret)
		lua_pushstring(luaState, filename);
	
	else
		lua_pushnil(luaState);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int luaAddToPlaylist(lua_State* luaState)
{
	const char* filename = luaL_checkstring(luaState, 1);
	Hippo_addToPlaylist(filename);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int luaGetPlaylistFiles(lua_State* luaState)
{
	int count;
	int offset = (uint32_t)luaL_checkint(luaState, 1);

	const char** files = Hippo_getPlaylistFiles(&count, offset);

	if (!files)
	{
		lua_pushnil(luaState);
		return 1;
	}

	lua_newtable(luaState);
	int top = lua_gettop(luaState);

	for (int i = 0; i < count; ++i)
	{
		const char* value = files[i]; 
		lua_pushnumber(luaState, i + 1);
		lua_pushstring(luaState, value);
		lua_settable(luaState, top);
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const luaL_Reg hippoLib[] =
{
	{ "addToPlaylist", luaAddToPlaylist },
	{ "getPlaylistFiles", luaGetPlaylistFiles },
	{ "openFileDialog", luaOpenFileDialog },
	{ "quit", luaQuit },
	{ 0, 0 },
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int registerHippoLib(lua_State* state)
{
	luaL_newlib(state, hippoLib);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoLua_registerLuaFunctions(struct lua_State* luaState)
{
	luaL_requiref(luaState, "hippo", registerHippoLib, 1);
    lua_pop(luaState, 1);  
}
