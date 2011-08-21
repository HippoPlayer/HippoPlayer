#include "HippoClassicTest.h"
#include "graphics/gui/HippoGui.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_drawClassic()
{
	HippoGui_begin();

	// Some initial drawing

	HippoGui_fill(0x00a0a0a0, 0, 0, 264, 136);
	HippoGui_fill(0x00ffffff, 25, 54, 1, 68);
	HippoGui_fill(0x00ffffff, 25, 54, 224, 1);
	HippoGui_fill(0x00000000, 25 + 224, 54, 1, 68);
	HippoGui_fill(0x00000000, 25, 62 + 60, 224, 1);
	HippoGui_fill(0x00aaaaaa, 26, 55, 157, 67);

	// todo: Add support for staticImage with coords
	HippoGui_beginHorizontalStackPanelXY(158, 56);
	HippoGui_staticImage("skins/classic/hippo.png");

	HippoGui_beginHorizontalStackPanelXY(0, 0);
	{
		HippoGui_staticImage("skins/classic/close.png");
		HippoGui_staticImage("skins/classic/upper_bar.png");
		HippoGui_staticImage("skins/classic/hide.png");
		HippoGui_staticImage("skins/classic/minimize.png");
	}

	HippoGui_beginHorizontalStackPanelXY(0, 30);
	{
		if (HippoGui_buttonImage("skins/classic/play.png"))
			printf("play!\n");
		if (HippoGui_buttonImage("skins/classic/open.png"))
			printf("open!\n");
	}

	HippoGui_end();
}


