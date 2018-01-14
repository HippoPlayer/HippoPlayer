#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoPlaybackPlugin {
	uint64_t version;
	const char* (*info)(void* userData);
	const char* (*trackInfo)(void* userData);
	const char* (*supported_extensions)();
	void* (*create)();
	int (*destroy)(void* userData);
	int (*open)(void* userData, const char* buffer);
	int (*close)(void* userData);
	int (*read_data)(void* userData, void* dest);
	int (*seek)(void* userData, int ms);
	int (*frame_size)(void* userData);
	int (*length)(void* userData);

	// Not to be used by plugin, owned by app
	void* privateData;

} HippoPlaybackPlugin;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define HIPPO_EXPORT __declspec(dllexport)
#else
#define HIPPO_EXPORT
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
}
#endif

