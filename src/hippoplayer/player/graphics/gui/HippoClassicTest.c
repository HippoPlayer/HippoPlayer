#include "HippoClassicTest.h"
#include "graphics/gui/HippoGui.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoGui_drawClassic()
{
	HippoGui_begin();

	HippoGui_beginHorizontalStackPanelXY(0, 40);
	{
		if (HippoGui_buttonImage("skins/classic/play.png"))
			printf("play!\n");
		//if (HippoGui_buttonImage("skins/classic/open.png"))
		//	printf("open!\n");
	}

	HippoGui_end();
}


