
#include "HippoGui.h"

///

HippoGuiState g_hippoGuiState;
static int s_controlId;

typedef enum ControlType
{
	TYPE_BUTTON,
	TYPE_STATIC,
	TYPE_STATIC_IMAGE,
} ControlType;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FillArea 
{
	uint32_t color;
} FillArea;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Image 
{
	struct ImageData* imageData;
	void* userData;

} StaticImage;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ControlInfo
{
	ControlType type;

	int x;
	int y;
	int width;
	int height;

	union 
	{
		FillArea fill;
		StaticImage staticImage;
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_begin()
{
	s_controlId = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_end()
{
}


