
#ifndef HIPPOAPI_H
#define HIPPOAPI_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

#ifdef _cplusplus
}
#endif

#endif

