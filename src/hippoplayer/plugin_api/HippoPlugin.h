
#ifndef HIPPOAPI_H
#define HIPPOAPI_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct LinearAllocator;

#ifdef _cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoPlaybackPlugin
{
	int version;
	const char* (*info)(void* userData);
	const char* (*trackInfo)(void* userData);
	const char* (*supportedExtensions)(void* userData);
	int (*create)(void* userData);
	int (*destroy)(void* userData);
	int (*open)(void* userData, const char* buffer);
	int (*close)(void* userData);
	int (*readData)(void* userData, void* dest, int size);
	int (*seek)(void* userData, int ms);

	void* userData;

} HippoPlaybackPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
typedef struct HippoDecrunchPlugin
{
	int version;

	int (*create)(void* userData);
	int (*destroy)(void* userData);
	const char** (*getFilelist)(void* userData); 
	void* unpack(void* userData, Allocator* allocator, void* identifier);

} HippoDecrunchPlugin;
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
}
#endif

#endif

