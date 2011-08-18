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

extern HippoGuiState g_hippoGuiState;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_beginVerticalStackPanelXY(int x, int y);
void HippoGui_beginHorizontalStackPanelXY(int x, int y);

void HippoGui_beginVerticalStackPanel();
void HippoGui_beginHorizontalStackPanel();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Different controlls and gui functions

void HippoGui_fill(uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

bool HippoGui_button(const char* text, int x, int y);
bool HippoGui_buttonImage(const char* text, int x, int y);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

