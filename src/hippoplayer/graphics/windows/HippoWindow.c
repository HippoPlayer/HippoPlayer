#include "../HippoWindow.h"
#include <core/memory/LinearAllocator.h>
#include <windows.h>

typedef struct HippoWindow
{
	HWND hwnd;
	HippoWindowRect rect;

} HippoWindow;

HINSTANCE g_winInstance;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void registerClass(const char* name)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance= g_winInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0; 
	wcex.lpszClassName= name;
	wcex.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoWindow* HippoWindow_create(struct LinearAllocator* allocator, const char* name, const HippoWindow* parent, 
								const HippoWindowRect* rect)
{
	HWND hwnd;

	if (!registerClass(name))
	{
		// TODO: Proper error message here
		return 0;
	}

 	hWnd = CreateWindow(name, name, WS_OVERLAPPED | WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

 	if (!hwnd)
 	{
		// TODO: Proper error message here
		return 0;
 	}

	UpdateWindow(hWnd);
	SetWindowPos(hWnd, NULL, rect->x, rect-Y, rect->sizeX, rect->sizeY, SWP_SHOWWINDOW);

	HippoWindow* window = LinearAllocator_allocZero(allocator, HippoWindow);
	window->hwnd = hwnd;
	window->rect = *rect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_destroy(HippoWindow* window)
{
	HIPPO_ASSERT(window);
	HIPPO_ASSERT(window->hwnd);

	if (!window->hwnd)
		return;

	DestroyWindow(window->hwnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_fullBlit(HippoWindow* window, const void* data);
void HippoWindow_partialBlit(HippoWindow* window, const void* data, const HippoWindowRect* rect); 


