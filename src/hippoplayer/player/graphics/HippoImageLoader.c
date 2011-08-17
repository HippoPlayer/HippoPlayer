#include "HippoImageLoader.h"
#include <stb_image/stb_image.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HippoImageLoader_loadFile(HippoImage* image, const char* filename)
{
	const uint32_t* imageData = (const uint32_t*)stbi_load(filename, &image->width, &image->height, &image->comp, 0);

	if (imageData)
	{
		// TODO: Report errer
		image->data = imageData;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HippoImageLoader_loadBuffer(HippoImage* image, const void* buffer, size_t size)
{
	const uint32_t* imageData = (const uint32_t*)stbi_load_from_memory(buffer, (int)size, &image->width, &image->height, &image->comp, 0);

	if (imageData)
	{
		// TODO: Report errer
		image->data = imageData;
		return true;
	}

	return false;
}


