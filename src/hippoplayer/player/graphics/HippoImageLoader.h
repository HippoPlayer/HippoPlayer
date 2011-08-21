
#ifndef HIPPOIMAGE_H
#define HIPPOIMAGE_H

#include <core/Types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoImage
{
	const uint32_t* data;
	void* userData;	// used by platform implementation (quartz 2D on Mac OS X for example)
	int width;
	int height;
	int comp;
	bool hasAlpha;

} HippoImage;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HippoImageLoader_loadFile(HippoImage* image, const char* filename);
bool HippoImageLoader_loadBuffer(HippoImage* image, const void* buffer, size_t size);

#endif

