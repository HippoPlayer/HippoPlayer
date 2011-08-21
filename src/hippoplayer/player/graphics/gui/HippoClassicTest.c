#include "HippoClassicTest.h"
#include "graphics/gui/HippoGui.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_drawClassic()
{
	HippoGui_begin();

	// Some initial drawing
	
	HippoGui_fill(0x00a0a0a0, 0, 0, 264, 136);

	HippoGui_drawBorder(0x0000000, 0x00ffffff, 2, 14, 256, 20);
	HippoGui_drawBorder(0x00ffffff, 0x00000000, 25, 65, 224, 67);
	HippoGui_fill(0x00aaaaaa, 26, 66, 157, 67);

	// todo: Add support for staticImage with coords
	HippoGui_beginHorizontalStackPanelXY(158, 66);
	HippoGui_staticImage("skins/classic/hippo.png");

	HippoGui_beginHorizontalStackPanelXY(0, 0);
	{
		HippoGui_staticImage("skins/classic/close.png");
		HippoGui_staticImage("skins/classic/upper_bar.png");
		HippoGui_staticImage("skins/classic/hide.png");
		HippoGui_staticImage("skins/classic/minimize.png");
	}

	HippoGui_beginHorizontalStackPanelXY(0, 38);
	{
		if (HippoGui_buttonImage("skins/classic/play.png"))
			printf("play!\n");
		if (HippoGui_buttonImage("skins/classic/open.png"))
			printf("open!\n");
	}

	HippoGui_end();
}


