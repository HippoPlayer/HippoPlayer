#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics/HippoWindow.h"

#if defined(HIPPO_WIN32)
#include <windows.h>
#endif

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
	struct HippoWindow* window;
#endif

	rect.x = 50;
	rect.y = 50;
	rect.width = 250;
	rect.height = 250;

	window = HippoWindow_create(0, "foo", 0, &rect);

	while (!HippoWindow_isClosed())
	{
		HippoWindow_updateEvents();
	}

	return 0;
}
