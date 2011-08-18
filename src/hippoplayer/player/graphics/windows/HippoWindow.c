#include "../HippoWindow.h"
#include <core/memory/LinearAllocator.h>
#include <core/debug/Assert.h>
#include <graphics/gui/HippoGui.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoWindow
{
	uint32_t* frameBuffer;
	HWND hwnd;
	HDC hdc;
	HippoWindowRect rect;
	BITMAPINFO bitmapHeader;

} HippoWindow;

HINSTANCE g_winInstance;
HippoWindow* g_window = 0; // temp
int isRunning = true;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	int result = 0;

	switch (message)
	{
		case WM_PAINT:
		{
			HippoWindow* window = (HippoWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (window)
			{
				StretchDIBits(window->hdc, 0, 0, window->rect.width, window->rect.height, 
										0, 0, window->rect.width, window->rect.height, 
										window->frameBuffer, &window->bitmapHeader, DIB_RGB_COLORS, SRCCOPY);
				ValidateRect(window->hwnd, NULL);
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			g_hippoGuiState.mousex = LOWORD(lParam); 
			g_hippoGuiState.mousey = HIWORD(lParam); 
			break;
		}

		case WM_LBUTTONDOWN:
		{
			if (wParam == MK_LBUTTON)
				g_hippoGuiState.mouseDown = 1;
			break;
		}

		case WM_LBUTTONUP:
		{
			if (wParam == MK_LBUTTON)
				g_hippoGuiState.mouseDown = 0;
			break;
		}

		case WM_KEYDOWN:
		{
			if ((wParam & 0xFF) != 27) 
				break;
		}


		case WM_CLOSE:
		{
			isRunning = false;
			// TODO: Proper cleanup
			ExitProcess(0);
			break;
		}

		default:
		{
			result = DefWindowProc(hWnd,message,wParam,lParam);
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static ATOM registerClass(const char* name)
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
	wcex.hIconSm = 0;//LoadIcon(instance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoWindow* HippoWindow_create(struct LinearAllocator* allocator, const char* name, const HippoWindow* parent, 
								const HippoWindowRect* rect)
{
	uint i;
	HippoWindow* window = 0;
	HWND hwnd;

	if (!registerClass(name))
	{
		// TODO: Proper error message here
		return 0;
	}

 	hwnd = CreateWindow(name, name, WS_OVERLAPPED | WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, g_winInstance, NULL);

 	if (!hwnd)
 	{
		// TODO: Proper error message here
		return 0;
 	}

	UpdateWindow(hwnd);
	SetWindowPos(hwnd, NULL, rect->x, rect->y, rect->width, rect->height, SWP_SHOWWINDOW);

	window = malloc(sizeof(HippoWindow)); //LinearAllocator_allocZero(allocator, HippoWindow);
	window->frameBuffer = (uint32_t*)malloc(rect->width * rect->height * 4);

	for (i = 0; i < rect->width * rect->height; ++i)
	{
		window->frameBuffer[i] = rand();
	}

	window->bitmapHeader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	window->bitmapHeader.bmiHeader.biPlanes = 1;
	window->bitmapHeader.bmiHeader.biBitCount = 32;
	window->bitmapHeader.bmiHeader.biCompression = BI_BITFIELDS;
	window->bitmapHeader.bmiHeader.biWidth = rect->width;
	window->bitmapHeader.bmiHeader.biHeight = -rect->height; 
	((uint32_t*)window->bitmapHeader.bmiColors)[0] = 0x00FF0000;
	((uint32_t*)window->bitmapHeader.bmiColors)[1] = 0x0000FF00;
	((uint32_t*)window->bitmapHeader.bmiColors)[2] = 0x000000FF;

	window->hdc = GetDC(hwnd);
	window->hwnd = hwnd;
	window->rect = *rect;

	g_window = window;

	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)window);

	return window;
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

int HippoWindow_isClosed()
{
	return !isRunning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_updateEvents()
{
	MSG message;
	HWND wnd = g_window->hwnd;

	InvalidateRect(wnd, NULL, TRUE);
	SendMessage(wnd, WM_PAINT, 0, 0);

	while (PeekMessage(&message, wnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_fullBlit(HippoWindow* window, const void* data);
void HippoWindow_partialBlit(HippoWindow* window, const void* data, const HippoWindowRect* rect); 


