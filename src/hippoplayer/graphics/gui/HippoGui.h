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

enum HippoSliderDirection
{
	SLIDERDIRECTION_HORIZONTAL,
	SLIDERDIRECTION_VERTICAL,
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

	// todo: Use union with all data instead
	int sliderThumbX;
	int sliderThumbY;
	int sliderThumbWidth;
	int sliderThumbHeight;


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
// Not really a GUI function per see but will do for now

void Hippo_addToPlaylist(const char* filename);
const char** Hippo_getPlaylistFiles(int* count, uint32_t offset);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Different controlls and gui functions

void HippoGui_staticImage(const char* filename);
void HippoGui_fill(uint32_t color, int x, int y, int w, int h);
void HippoGui_drawBorder(uint32_t color0, uint32_t color1, int x, int y, int w, int h);
void HippoGui_textLabelXY(const char* text, int x, int y);
void HippoGui_textLabel(const char* text);

// TODO: Make this much more configurable
void HippoGui_slider(int x, int y, int w, int h, int start, int end, enum HippoSliderDirection dir, int* value);

bool HippoGui_buttonCoords(const char* text, int x, int y);
bool HippoGui_buttonCoordsImage(const char* text, int x, int y);

bool HippoGui_button(const char* text);
bool HippoGui_buttonImage(const char* filename);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

