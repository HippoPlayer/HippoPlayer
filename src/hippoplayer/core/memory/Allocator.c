#include "Allocator.h"
#include "core/debug/Assert.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t* g_memoryStart = 0;
static uint8_t* g_memoryCurrent = 0;
static uint8_t* g_memoryEnd = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static size_t alignUp(size_t value, uint32_t alignment)
{
	intptr_t ptr = value;	
	uint32_t bitMask = (alignment - 1);
	uint32_t lowBits = ptr & bitMask;
	uint32_t adjust = ((alignment - lowBits) & bitMask);
	return adjust;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Allocator_setStart(void* start, uint32_t totalSize, uint32_t alignment)
{
	g_memoryStart = g_memoryCurrent = (uint8_t*)start + alignUp((intptr_t)start, alignment);
	g_memoryEnd = g_memoryStart + totalSize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Store the debugname in a separete tracking array

void* Allocator_alloc(const char* debugName, uint32_t size, uint32_t alignment)
{
	uint8_t* retData;

	HIPPO_ASSERT_DESC(size > 0, "Trying to allocate %s with size of 0", debugName);

	g_memoryCurrent += alignUp((uintptr_t)g_memoryCurrent, alignment);

	HIPPO_ASSERT((g_memoryCurrent + size) < g_memoryEnd);

	retData = g_memoryCurrent; 
	g_memoryCurrent += size;

	return (void*)retData;
}

