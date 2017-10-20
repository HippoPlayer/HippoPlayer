
#ifndef HIPPOAPI_H
#define HIPPOAPI_H

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct LinearAllocator;

#ifdef _cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoPlaybackBuffer
{
	unsigned char* data;
	unsigned int readOffset;
	unsigned int frameSize;
	unsigned int frameMaxSize;
} HippoPlaybackBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoPlaybackPlugin
{
	uint64_t version;
	const char* (*info)(void* userData);
	const char* (*trackInfo)(void* userData);
	const char* (*supportedExtensions)(void* userData);
	void* (*create)();
	int (*destroy)(void* userData);
	int (*open)(void* userData, const char* buffer);
	int (*close)(void* userData);
	int (*readData)(void* userData, void* dest);
	int (*seek)(void* userData, int ms);
	int (*frameSize)(void* userData);

	// Not to be used by plugin, owned by app
	void* privateData;

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

//HippoPlaybackPlugin* getPlugin();


#ifdef _WIN32
#define HIPPO_EXPORT __declspec(dllexport)
#else
#define HIPPO_EXPORT
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
}
#endif

#endif

