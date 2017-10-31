#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t WUHandle;
struct Wrui;
struct WUPainter;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUPos {
	float x, y;
} WUPos;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WURect {
	float x, y, width, height;
} WURect;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUColor {
	float r, g, b, a;
} WUColor;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*WUPaintEvent)(const struct WUPainter* painter, void* user_data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUFont {
	int (*set_size)(const struct WUFont* font, int size);
	int (*set_font_from_memory)(struct WUFont* font, void* data, int size);
	int (*set_font_from_filename)(struct WUFont* font, const char* filename, int len);
} WUFont;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUPainter {
	WUFont* (*create_font)(void);
	void (*draw_text)(const struct WUPainter* painter, WUPos pos, WUColor color, const char* text, int len, const WUFont* font);
	void (*draw_rect)(const struct WUPainter* painter, WURect rect, WUColor color);
} WUPainter;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUWindowFuncs {
    WUHandle (*create)(WUHandle parent);

    // Window overrides
	void (*set_paint_event)(WUHandle window, void* user_data, WUPaintEvent event);

} WUWindowFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUWidgetFuncs {
    WUHandle (*button_create)(WUHandle parent);

    // Shared WUWidgetFuncs
    void (*set_size)(WUHandle handle, int x, int y);
} WUWidgetFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUMainWindowFuncs {
    WUHandle (*create)(void);
} WUMainWindowFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct WUApplicationFuncs {
    WUHandle (*create)(void);
    int (*run)(WUHandle handle);
} WUApplicationFuncs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Wrui {
	uint64_t api_version;

    // Application functios
	const WUApplicationFuncs* application_funcs;

	// Window functions
	const WUWindowFuncs* window_funcs;

    // Main Window functions
	const WUMainWindowFuncs* main_window_funcs;

	// Widget Functinos
	const WUWidgetFuncs* widget_funcs;

} Wrui;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WRUI_VERSION_CREATE(major, minor, sub) ((((uint64_t)major) << 32) | (minor << 16) | (sub))
#define WRUI_VERSION WRUI_VERSION_CREATE(0, 0, 1)

#ifdef _WIN32
#define WRUI_EXPORT __declspec(dllexport)
#else
#define WRUI_EXPORT
#endif

// Should be the only exported symbol

extern Wrui* wrui_get(void);

#ifdef __cplusplus
}
#endif

