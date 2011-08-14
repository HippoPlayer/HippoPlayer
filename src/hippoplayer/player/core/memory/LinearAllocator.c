#include "LinearAllocator.h"
#include <string.h>
#include <core/debug/Assert.h>
#include <stdlib.h>

static LinearAllocator g_scratchPad;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* LinearAllocator_allocAligned(LinearAllocator* state, uint32_t size, uint32_t alignment)
{
	void* retData;

	// Align the pointer to the current 

	intptr_t ptr = (intptr_t)state->current;	
	uint32_t bitMask = (alignment - 1);
	uint32_t lowBits = ptr & bitMask;
	uint32_t adjust = ((alignment - lowBits) & bitMask);

	// adjust the pointer to correct alignment

	state->current += adjust;

	HIPPO_ASSERT(((intptr_t)state->current - (intptr_t)state->start) + size < state->maxSize);

	retData = (void*)state->current;
	state->current += size;
	return retData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* LinearAllocator_allocAlignedZero(LinearAllocator* allocator, uint32_t size, uint32_t alignment)
{
	void* mem = LinearAllocator_allocAligned(allocator, size, alignment);
	memset(mem, 0, size);
	return mem;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LinearAllocator_setScratchPad(void* data, uint32_t size)
{
	LinearAllocator_create(&g_scratchPad, data, size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LinearAllocator* LinearAllocator_getScratchPad()
{
	return &g_scratchPad;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* LinearAllocator_allocString(LinearAllocator* allocator, const char* value)
{
	const size_t len = strlen(value) + 1;
	char* mem = LinearAllocator_allocArray(allocator, char, (uint32_t)len); 
	memcpy(mem, value, len);
	return mem;
}

