#ifndef HIPPOWINDOW_H
#define HIPPOWINDOW_H

#include "core/Types.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoWindowRect
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
} HippoWindowRect;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoWindow;
struct LinearAllocator;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoWindow* HippoWindow_create(struct LinearAllocator* allocator, const char* name, const struct HippoWindow* parent, 
									   const HippoWindowRect* rect);
void HippoWindow_destroy(struct HippoWindow* window);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_fullBlit(struct HippoWindow* window, const void* data);
void HippoWindow_partialBlit(struct HippoWindow* window, const void* data, const struct HippoWindowRect* rect); 
void HippoWindow_updateEvents();
int HippoWindow_isClosed();

#endif // HIPPOWINDOW_H

