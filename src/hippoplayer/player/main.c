#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics/HippoWindow.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"

#if defined(HIPPO_WIN32)
#include <windows.h>
#endif

extern void HippoGui_drawClassic();

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

	return 0;
}
