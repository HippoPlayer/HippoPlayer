
#ifndef HIPPOAPI_H
#define HIPPOAPI_H

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
	int version;
	const char* (*info)(void* userData);
	const char* (*trackInfo)(void* userData);
	const char** (*supportedExtensions)(void* userData);
	void* (*create)();
	int (*destroy)(void* userData);
	int (*open)(void* userData, const char* buffer);
	int (*close)(void* userData);
	int (*readData)(void* userData, HippoPlaybackBuffer* dest);
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

HippoPlaybackPlugin* getPlugin();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
}
#endif

#endif

