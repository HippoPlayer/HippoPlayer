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

int HippoMain_create();
void HippoMain_destroy();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_winInstance = hInstance;

	AllocConsole();
	freopen("CONOUT$","wb",stdout);

	if (!HippoMain_create())
		return 0;

	while (!HippoWindow_isClosed())
	{
		HippoWindow_updateEvents();
	}

	return 0;
}

