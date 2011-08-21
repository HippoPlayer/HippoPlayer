
#include "HippoGui.h"
#include <core/debug/Assert.h>
#include <graphics/HippoImageLoader.h>
#include <graphics/HippoWindow.h>
#include <string.h>
#include <stdio.h>
#include <sys/syslimits.h>

///

enum
{
	MAX_CONTROLS = 1024,
	MAX_IMAGES = 1024,
};

HippoGuiState g_hippoGuiState;
uint32_t s_controlId;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct GuiPlacementInfo
{
	enum GuiPlacementState
	{
		PLACEMENTSTATE_NONE,
		PLACEMENTSTATE_HORIZONAL,
		PLACEMENTSTATE_VERTICAL,
	};

	enum GuiPlacementState state;
	int x;
	int y;

} GuiPlacementInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static GuiPlacementInfo g_placementInfo;
HippoControlInfo g_controls[MAX_CONTROLS];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_beginHorizontalStackPanelXY(int x, int y)
{
	g_placementInfo.state = PLACEMENTSTATE_HORIZONAL;
	g_placementInfo.x = x;
	g_placementInfo.y = y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_begin()
{
	s_controlId = 0;
	memset(&g_placementInfo, 0, sizeof(GuiPlacementInfo));
}

struct LoadedImages
{
	char filename[PATH_MAX][MAX_IMAGES];
	HippoImage image[MAX_IMAGES];
	uint32_t count;
} g_loadedImages;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_reset()
{
	memset(&g_loadedImages, 0, sizeof(struct LoadedImages)); // not really needed but good when debugging
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoImage* loadImage(const char* filename)
{
	uint32_t index = g_loadedImages.count;

	HIPPO_ASSERT(filename);

	// TODO: Hash filenames?

	for (uint i = 0; i != index; ++i)
	{
		if (!strcmp(filename, g_loadedImages.filename[i]))
			return &g_loadedImages.image[i];
	}

	// Load new image

	if (!HippoImageLoader_loadFile(&g_loadedImages.image[index], filename))
	{
		printf("Unable to load %s\n", filename);
		return 0;
	}	
	
	strcpy(g_loadedImages.filename[index], filename); g_loadedImages.count++;

	return &g_loadedImages.image[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HippoGui_regionHit(const HippoControlInfo* control)
{
	printf("mouse xy %d %d\n", g_hippoGuiState.mousex,g_hippoGuiState.mousey);
	printf("rect %d %d %d %d\n", control->x, control->y, control->width, control->height); 

	if (g_hippoGuiState.mousex < control->x ||
		g_hippoGuiState.mousey < control->y ||
		g_hippoGuiState.mousex >= control->x + control->width ||
		g_hippoGuiState.mousey >= control->y + control->height)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HippoGui_buttonImage(const char* filename)
{
	uint32_t controlId = 0;
	HippoControlInfo* control = 0; 
	HippoImage* image = loadImage(filename);

	if (!image)
		return false;

	// Setup the control
	
	controlId = s_controlId++;
	control = &g_controls[controlId];
	control->type = DRAWTYPE_IMAGE;
	control->x = g_placementInfo.x;
	control->y = g_placementInfo.y;
	control->width = image->width;
	control->height = image->height;
	control->imageData = image; 

	switch (g_placementInfo.state)
	{
		case PLACEMENTSTATE_NONE :
		{
			break;
		}

		case PLACEMENTSTATE_HORIZONAL :
		{
			g_placementInfo.x += image->width;
			break;
		}

		case PLACEMENTSTATE_VERTICAL :
		{
			g_placementInfo.y += image->height;
			break;
		}
	}

	return HippoGui_regionHit(control);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_end()
{
	HippoWindow_refresh();
}


