#ifndef LINEARALLOCATOR_H 
#define LINEARALLOCATOR_H  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <core/Types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct LinearAllocator
{
	uint8_t* start;
	uint8_t* current;
	uint32_t maxSize;
} LinearAllocator;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct LinearAllocatorRewindPoint
{
	uint8_t* pointer;
} LinearAllocatorRewindPoint;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HIPPO_INLINE void LinearAllocator_create(LinearAllocator* state, void* data, uint32_t size)
{
	state->start = state->current = (uint8_t*)data;
	state->maxSize = size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \todo Document

static HIPPO_INLINE void LinearAllocator_reset(LinearAllocator* state)
{
	state->current = state->start;
}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \todo Document

void* LinearAllocator_allocAligned(LinearAllocator* state, uint32_t size, uint32_t alignment);
void* LinearAllocator_allocAlignedZero(LinearAllocator* state, uint32_t size, uint32_t alignment);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief Makes it possible to get a rewindpoint to rewind the memory back to the memory position at the point
//! when the function was called (use LinerarAllocator_rewind) to rewind back.
//! Notice that "rewind" in this sense is just to move a pointer back. It actually doesn't do anything with the memory
//! \param[in] state the current instance of LinearAllocator (as setup by LinearAllocator_create)
//! \param[in] rewindPoint holds the restorpoint 

static HIPPO_INLINE LinearAllocatorRewindPoint LinearAllocator_getRewindPoint(const LinearAllocator* allocator) 
{
	LinearAllocatorRewindPoint rewindPoint;
	rewindPoint.pointer = allocator->current;
	return rewindPoint;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// \brief Rewinds the allocator to a given rewindpoint
//! \param[in] rewindPoint Rewindpoint to rewind back to 
//! \param[in] state the current instance of LinearAllocator (as setup by LinearAllocator_create)

static HIPPO_INLINE void LinearAllocator_rewind(LinearAllocator* state, LinearAllocatorRewindPoint rewindPoint)
{
	state->current = rewindPoint.pointer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// \brief Rewinds the allocator to a given rewindpoint

static HIPPO_INLINE ptrdiff_t LinearAllocator_getCursor(const LinearAllocator* allocator)
{
	return ((intptr_t)allocator->current) - ((intptr_t)allocator->start);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// \brief Gets a "scratch" pad that the user can use to allocate temporary memory. Notice that its up to the user
// to rewind the allocator back \linkLinearAllocator_getRewindPoint\endlink and \linkLinearAllocator_rewind\endlink 

LinearAllocator* LinearAllocator_getScratchPad();
void LinearAllocator_setScratchPad(void* data, uint32_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper macros

#define LinearAllocator_alloc(state, type) (type*)LinearAllocator_allocAligned(state, sizeof(type), HIPPO_ALIGNOF(type))
#define LinearAllocator_allocZero(state, type) (type*)LinearAllocator_allocAlignedZero(state, sizeof(type), HIPPO_ALIGNOF(type))
#define LinearAllocator_allocArray(state, type, count) (type*)LinearAllocator_allocAlignedZero(state, sizeof(type) * count, \
										  HIPPO_ALIGNOF(type))
#define LinearAllocator_allocArrayZero(state, type, count) (type*)LinearAllocator_allocAlignedZero(state, sizeof(type) * count, HIPPO_ALIGNOF(type))


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* LinearAllocator_allocString(LinearAllocator* allocator, const char* name);

#endif

