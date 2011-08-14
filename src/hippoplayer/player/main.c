#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics/HippoWindow.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	struct HippoWindow* window = HippoWindow_create(0, "foo", 0, 0);

	while (!HippoWindow_isClosed())
	{
		HippoWindow_updateEvents();
	}

	return 0;
}
