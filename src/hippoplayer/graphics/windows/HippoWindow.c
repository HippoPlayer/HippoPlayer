#include "../HippoWindow.h"
#include "core/memory/LinearAllocator.h"
#include "core/debug/Assert.h"
#include "graphics/gui/HippoGui.h"
#include "graphics/gui/HippoFont.h"

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

static void updateWindow(HippoWindow* window)
{
	uint32_t i;
    RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    HBRUSH hbrBkGnd;
    HFONT hfntOld;
	uint32_t controlCount = s_controlId; 
	HippoControlInfo* controls = (HippoControlInfo*)&g_controls;

	// TODO: Create the offline screen once

	GetClientRect(window->hwnd, &rc);
    hdcMem = CreateCompatibleDC(window->hdc);
    hbmMem = CreateCompatibleBitmap(window->hdc, rc.right-rc.left, rc.bottom-rc.top);
    hbmOld = SelectObject(hdcMem, hbmMem);

	for (i = 0; i < controlCount; ++i)
	{
		HippoControlInfo* control = &g_controls[i]; 
		int y_pos = control->y;

		switch (controls[i].type)
		{
			case DRAWTYPE_NONE :
				break;

			case DRAWTYPE_FILL :
			{
				// TODO: Rewrite with FillRect
				TRIVERTEX vertex[2];
				GRADIENT_RECT gRect;
				uint32_t color = control->color;
				uint16_t a = ((color >> 24) & 0xff) << 8;
				uint16_t r = ((color >> 16) & 0xff) << 8;
				uint16_t g = ((color >> 8) & 0xff) << 8;
				uint16_t b = ((color >> 0) & 0xff) << 8;

				vertex[0].x = control->x;
				vertex[0].y = control->y;
				vertex[0].Red  = r;
				vertex[0].Green = g;
				vertex[0].Blue  = b;
				vertex[0].Alpha = 0;

				vertex[1].x = control->x + control->width;
				vertex[1].y = control->y + control->height;
				vertex[1].Red  = r;
				vertex[1].Green = g;
				vertex[1].Blue  = b;
				vertex[1].Alpha = 0;

				gRect.UpperLeft  = 0;
				gRect.LowerRight = 1;

				// Draw a shaded rectangle. 
				GradientFill(hdcMem, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
				break;
			}

			// this will be hacky

			case DRAWTYPE_TEXT :
			{
				break;
			}

			case DRAWTYPE_IMAGE :
			{
				/*
				HIPPO_ASSERT(control->imageData);

				// if we have no userData, we need to initialize it with CimageDataRef

				if (!control->imageData->userData)
				{
					CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
					CGDataProviderRef provider = CGDataProviderCreateWithData(
						NULL, control->imageData->data, control->imageData->width * control->imageData->height * 3, NULL);

					CGImageRef img = CGImageCreate(
						control->imageData->width, control->imageData->height, 8, 24,
						control->imageData->width * 3, space, kCGImageAlphaNoneSkipFirst,
						provider, NULL, false, kCGRenderingIntentDefault);

					control->imageData->userData = (void*)img; 
					CGColorSpaceRelease(space);
					CGDataProviderRelease(provider);
				}

				CGContextDrawImage(context, CGRectMake(control->x, y_pos, control->width, control->height), 
								  (CGImageRef)control->imageData->userData);
				
				*/
				break;	
			}
		}
	}

	// Blit ofscreen to front
	BitBlt(window->hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	int result = 0;
	HippoWindow* window = (HippoWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
		case WM_PAINT:
		{
			updateWindow(window);
			break;
			/*
			if (window)
			{
				StretchDIBits(window->hdc, 0, 0, window->rect.width, window->rect.height, 
										0, 0, window->rect.width, window->rect.height, 
										window->frameBuffer, &window->bitmapHeader, DIB_RGB_COLORS, SRCCOPY);
				ValidateRect(window->hwnd, NULL);
			}
			*/
		}
		
		case WM_ERASEBKGND:
			return (LRESULT)1; // Say we handled it.


		case WM_MOUSEMOVE:
		{
			g_hippoGuiState.mousex = LOWORD(lParam); 
			g_hippoGuiState.mousey = HIWORD(lParam); 
			HippoLua_updateScript();
			break;
		}

		case WM_LBUTTONDOWN:
		{
			if (wParam == MK_LBUTTON)
				g_hippoGuiState.mouseDown = 1;
			HippoLua_updateScript();
			break;
		}

		case WM_LBUTTONUP:
		{
			if (wParam == MK_LBUTTON)
				g_hippoGuiState.mouseDown = 0;
			HippoLua_updateScript();
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

void Hippo_quit()
{
	HippoWindow_destroy(g_window);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoWindow_isClosed()
{
	return !isRunning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_refresh()
{
	UpdateWindow(g_window->hwnd);
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


