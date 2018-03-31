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

typedef enum HippoFileSeek {
	HippoFileSeek_Start,
	HippoFileSeek_Current,
	HippoFileSeek_End,
} HippoFileSeek;

typedef void* HippoIoHandle;

typedef struct HippoIoResult {
	const char* error_message;
	int status;
} HippoIoResult;

struct HippoApiPrivData;
typedef int64_t HippoIoErrorCode;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
typedef struct HippoIoAPI {
	int (*exists)(struct HippoApiPrivData* priv_data, const char* filename);

	HippoIoErrorCode (*read_file_to_memory)(struct HippoApiPrivData* priv_data, const char* filename, void** dest, uint64_t* size);
	HippoIoErrorCode (*free_file_to_memory)(struct HippoApiPrivData* priv_data, void* dest);


    // Io functions for more control
	HippoIoErrorCode (*open)(struct HippoApiPrivData* priv_data, const char* target, HippoIoHandle* handle);
	HippoIoErrorCode (*close)(HippoIoHandle handle);
	HippoIoErrorCode (*size)(HippoIoHandle handle, uint64_t* res);
	HippoIoErrorCode (*read)(HippoIoHandle handle, void* dest, int64_t size);
	HippoIoErrorCode (*seek)(HippoIoHandle handle, HippoFileSeek type, int64_t step);

	struct HippoApiPrivData* priv_data;
} HippoFileAPI;

#define HIPPO_FILE_API_VERSION 1

struct HippoMetadataAPIPrivData;

enum HippoMetaEncoding {
	HippoMetaEncoding_UTF8,
	HippoMetaEncoding_ShiftJS2,
};

enum HippoMetadataResult {
	HippoMetadataResult_KeyNotFound = 0,
	HippoMetadataResult_UnableToMakeQuery = -1,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoMetadataAPI {
    //
    // Get a string key associated with buffer.
    //
    // priv_data = send in priv_data pointer from this struct.
    // buffer = filename, url/stream, etc associated with the key
    // type = Key to retrive
    // error_code = HippoMetadataResult (may be NULL)
    // Returns data for the key. If there is an error (such as key not found) the return value will be NULL and
    // the error code will be updated.
    //
    const char* (*get_key)(
    	struct HippoMetadataAPIPrivData* priv_data,
    	const char* buffer,
    	const char* type,
    	int* error_code);
    //
    // Set a key associated with buffer.
    //
    // priv_data = send in priv_data pointer from this struct.
    // buffer = filename, url/stream, etc associated with the key
    // value = value to set with the key
    // type = Key to retrive
    //
    // HippoMetadataResult
    //
    int (*set_key)(
    	struct HippoMetadataAPIPrivData* priv_data,
    	const char* buffer,
    	uint32_t sub_song,
    	const char* value,
    	const char* type);

    //
    // Set a key associated with buffer with specific ecoding.
    //
    // In some cases the text you want to set may not be UTF-8 and this will convert from another encoding for you as HippoPlayer
    // uses utf8 for all strings internally
    //
    // priv_data = send in priv_data pointer from this struct.
    // buffer = filename, url/stream, etc associated with the key
    // value = value to set with the key
    // type = Key to retrive
    //
    int (*set_key_with_encoding)(
    	struct HippoMetadataAPIPrivData* priv_data,
    	const char* buffer,
    	uint32_t sub_song,
    	const char* value,
    	const char* type,
    	enum HippoMetaEncoding encoding);

	struct HippoMetadataAPIPrivData* priv_data;
} HippoMetadataAPI;

#define HippoMetadata_set_key(api, buffer, sub_song, value, type) api->set_key(api->priv_data, buffer, sub_song, value, type)
#define HippoMetadata_set_key_with_encoding(api, buffer, type, value, encode_type, error_code) api->set_key(api->priv_data, buffer, type, value, encode_type, error_code)

#define HippoMetadata_get_key(api, buffer, type, error_code) api->get_key(api->priv_data, buffer, type, error_code)

struct HippoServicePrivData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoServiceAPI {
	HippoFileAPI* (*get_io_api)(struct HippoServicePrivData* private_data, int api_version);
	HippoMetadataAPI* (*get_metadata_api)(struct HippoServicePrivData* private_data, int api_version);
	struct HippoServicePrivData* private_data;
} HippoServiceAPI;

#define HippoServiceAPI_get_io_api(api, version) api->get_io_api(api->private_data, version)
#define HippoServiceAPI_get_metadata_api(api, version) api->get_metadata_api(api->private_data, version)

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
	enum HippoProbeResult (*probe_can_play)(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size);

	// Returns a comma separated list of supported extensions
	const char* (*supported_extensions)();

	// Create an instace of the plugin. The user is expected to return an instance data poniter
	// that will get passed to the other callback functions.
	// Also a services pointer is passed down to create function that has various
	// services that the plugin can use (such as FileIO, metadata registration and more)
	// see the HippoServicesAPI documentation for mor info
	void* (*create)(HippoServiceAPI* services);

	// Destroy the instance of the plugin. It's expected that the user will free
	// the user_data pointer at this point as it won't be used anymore.
	int (*destroy)(void* user_data);

	// Opens a buffer to be ready for playback. Buffer may be a file/archived/file or a file or a network resource.
	// Use the HippoFileAPI that can be optained from services to load the data
	int (*open)(void* user_data, const char* buffer);

	// Closes the file buffer that was opend in open. Notice that the plugin isn't detroyed at this but but is
	// here for closing an open file/stream/etc
	int (*close)(void* user_data);

	// Called when Hippo is requesting sample output from the the plugin.
	// The plugin is allowed to return as many samples as it want's as long as it does't
	// go above max sample count
	int (*read_data)(void* user_data, void* dest, uint32_t max_sample_count);

	// Called when Hippo is requesting a new location in the data
	int (*seek)(void* user_data, int ms);
	// Not to be used by plugin, owned by app

	void* priv;

} HippoPlaybackPlugin;

#define HIPPO_PLAYBACK_PLUGIN_API_VERSION 1

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

