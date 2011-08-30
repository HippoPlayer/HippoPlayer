#ifndef HIPPOGUI_H
#define HIPPOGUI_H

#include <core/Types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoGuiState
{
	int mousex;
	int mousey;
	int mouseDown;

	int hotItem;
	int activeItem;

	int kbdItem;
	int keyEntered;
	int keyMod;

	int lastWidget;

} HippoGuiState;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoDrawType
{
	DRAWTYPE_NONE,
	DRAWTYPE_FILL,
	DRAWTYPE_IMAGE,
	DRAWTYPE_TEXT,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoControlInfo
{
	enum HippoDrawType type;

	int x;
	int y;
	int width;
	int height;

	uint32_t color;
	struct HippoImage* imageData;
	char* text;

} HippoControlInfo;

extern HippoGuiState g_hippoGuiState;
extern HippoControlInfo g_controls[1024];
extern uint32_t s_controlId;

struct HippoImage;
struct lua_State;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_registerLuaFunctions(struct lua_State* luaState);

void HippoGui_reset();
void HippoGui_begin();
void HippoGui_end();

void HippoGui_beginVerticalStackPanelXY(int x, int y);
void HippoGui_beginHorizontalStackPanelXY(int x, int y);

void HippoGui_beginVerticalStackPanel();
void HippoGui_beginHorizontalStackPanel();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Different controlls and gui functions

void HippoGui_staticImage(const char* filename);
void HippoGui_fill(uint32_t color, int x, int y, int w, int h);
void HippoGui_drawBorder(uint32_t color0, uint32_t color1, int x, int y, int w, int h);
void HippoGui_textLabel(int x, int y, const char* text);

bool HippoGui_buttonCoords(const char* text, int x, int y);
bool HippoGui_buttonCoordsImage(const char* text, int x, int y);

bool HippoGui_button(const char* text);
bool HippoGui_buttonImage(const char* filename);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

