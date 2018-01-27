#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum HippoProbeResult {
    HippoProbeResult_Supported,
    HippoProbeResult_Unsupported,
    HippoProbeResult_Unsure,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoPlaybackPlugin {
	uint64_t version;
	//
	// Ask the plugin if it can play some data. The plugin has to determine from the header if it supports the
	// file or not. The input data is at least 2048 bytes but can be less if the the total file is smaller.
	//
	// Notice that no user data is provided with this as the plugin instance hasn't actually been crated
	// The plugin must support to parse this data without custom data being setup.
	//
	enum HippoProbeResult (*probe_can_play)(const uint8_t* data, uint32_t data_size, uint64_t total_size);

	const char* (*info)(void* user_data);
	const char* (*trackInfo)(void* user_data);
	const char* (*supported_extensions)();
	void* (*create)();

	int (*destroy)(void* user_data);
	int (*open)(void* user_data, const char* buffer);
	int (*close)(void* user_data);
	int (*read_data)(void* user_data, void* dest);
	int (*seek)(void* user_data, int ms);
	int (*frame_size)(void* user_data);
	int (*length)(void* user_data);

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

