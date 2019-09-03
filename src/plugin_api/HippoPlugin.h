#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
extern "C"
{
#endif

struct RUPluginUI;

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoMessageEncode {
    struct HippoMessageEncode* priv_data;

	// Low-level APIs. Should only be used in case of custom commands
	uint32_t (*get_id)(struct HippoMessageEncode* handle);
	int (*write_formatted_blob)(struct HippoMessageEncode* handle, void* data, int size);
	int (*write_array_count)(struct HippoMessageEncode* handle, int count);
	int (*write_uint)(struct HippoMessageEncode* handle, uint64_t value);
	int (*write_str)(struct HippoMessageEncode* handle, const char* input);
	
	// Switch to next song in the playlist
	void (*playlist_next_song)();
	// Switch to previous song in the playlist
	void (*playlist_prev_song)();
	// Stop playing of the current song
	void (*stop_song)();
	// Play the current song
	void (*pause_song)();

} HippoMessageEncode;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoMessageDecode {
    struct HippoMessageDecode* priv_data;

	uint32_t (*get_id)(struct HippoMessageDecode* handle);
	const char* (*get_method)(struct HippoMessageDecode* handle);
	int (*get_raw_ptr)(struct HippoMessageDecode* handle, void** ptr, uint64_t* len);

	int (*read_array_count)(struct HippoMessageDecode* handle, int* count);
	int (*read_uint)(struct HippoMessageDecode* handle, uint64_t* value);
	int (*read_str_len)(struct HippoMessageDecode* handle, uint64_t* size);
	int (*read_str)(struct HippoMessageDecode* handle, char* dest);

} HippoMessageDecode;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined messages

#define HIPPO_PLAYLIST_NEXT_SONG "hippo_playlist_next_song"
#define HIPPO_PLAYLIST_PLAY_SONG "hippo_playlist_play_song"
#define HIPPO_PLAYLIST_STOP_SONG "hippo_playlist_stop_song"
#define HIPPO_PLAYLIST_PREV_SONG "hippo_playlist_prev_song"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plugins can use the MessageAPI to subscribe to events and post data that is being requested

typedef struct HippoMessageAPI {
    // Private internal data
	struct HippoMessageAPI* priv_data;
	// void (*subscribe)(struct HippoMessageAPI* priv_data, void* instance_data, const char* type);
	// void (*unsubscribe)(struct HippoMessageAPI* priv_data, void* instance_data, const char* type);

	struct HippoMessageEncode* (*begin_request)(struct HippoMessageAPI* priv_data, const char* id);
	struct HippoMessageEncode* (*begin_notification)(struct HippoMessageAPI* priv_data, const char* id);
	void (*end_message)(struct HippoMessageAPI* priv_data, HippoMessageEncode* message);

} HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HippoMessageAPI_begin_request(api, id) api->begin_request(api->priv_data, id)
#define HippoMessageAPI_end_message(api, message) api->end_message(api->priv_data, message)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoServicePrivData;

typedef struct HippoServiceAPI {
	HippoFileAPI* (*get_io_api)(struct HippoServicePrivData* private_data, int api_version);
	HippoMetadataAPI* (*get_metadata_api)(struct HippoServicePrivData* private_data, int api_version);
	HippoMessageAPI* (*get_message_api)(struct HippoServicePrivData* private_data, int api_version);
	struct HippoServicePrivData* private_data;
} HippoServiceAPI;

#define HippoServiceAPI_get_io_api(api, version) api->get_io_api(api->private_data, version)
#define HippoServiceAPI_get_metadata_api(api, version) api->get_metadata_api(api->private_data, version)
#define HippoServiceAPI_get_message_api(api, version) api->get_message_api(api->private_data, version)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// API Version of the plugin, always set this to HIPPO_PLAYBACK_PLUGIN_API_VERSION
//uint64_t api_version;

// Name of the plugin. This name should be unique or it may fail to load if there is a collision.
//const char* name;

// Scematic version of the plugin. If the version doesn't follow the rules of SemVersion it may fail to load.
//const char* version;

// Ask the plugin if it can play some data. The plugin has to determine from the header if it supports the
// file or not. The input data is at least 2048 bytes but can be less if the the total file is smaller.
//
// Notice that no user data is provided with this as the plugin instance hasn't actually been crated
// The plugin must support to parse this data without custom data being setup.
//enum HippoProbeResult (*probe_can_play)(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size);

// Returns a comma separated list of supported extensions
//const char* (*supported_extensions)();

// Create an instace of the plugin. The user is expected to return an instance data poniter
// that will get passed to the other callback functions.
// Also a services pointer is passed down to create function that has various
// services that the plugin can use (such as FileIO, metadata registration and more)
// see the HippoServicesAPI documentation for mor info
//
// create

// Destroy the instance of the plugin. It's expected that the user will free
// the user_data pointer at this point as it won't be used anymore.
//
// destroy
//
// Opens a buffer to be ready for playback. Buffer may be a file/archived/file or a file or a network resource.
// Use the HippoFileAPI that can be optained from services to load the data
//
// open
//
// Closes the file buffer that was opend in open. Notice that the plugin isn't detroyed at this but but is
// here for closing an open file/stream/etc
//
// close
//
// Called when Hippo is requesting sample output from the the plugin.
// The plugin is allowed to return as many samples as it want's as long as it does't
// go above max sample count
//
// read_data
//
// Called when Hippo is requesting a new location in the data
//
// seek
//
// Used to create configuration for this plugin. Can be NULL if no configuration.
//
// configure
//
// Serialize the state of the plugin and its settings (can be NULL)
//
// save
//
// Deserialzie the state of the plugin and it's setting (can be NULL)
//
// load
//
// Not to be used by plugin, owned by app
// priv;

struct HippoSaveAPI;
struct HippoLoadAPI;

typedef struct HippoPlaybackPlugin {
	uint64_t api_version;
	const char* name;
	const char* version;
	enum HippoProbeResult (*probe_can_play)(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size);
	const char* (*supported_extensions)();
	void* (*create)(const HippoServiceAPI* services);
	int (*destroy)(void* user_data);
	int (*open)(void* user_data, const char* buffer);
	int (*close)(void* user_data);
	int (*read_data)(void* user_data, void* dest, uint32_t max_sample_count);
	int (*seek)(void* user_data, int ms);
	int (*configure)(void* user_data, struct RUPluginUI* ui_funcs);
	int (*save)(void* user_data, struct HippoSaveAPI* save_api);
	int (*load)(void* user_data, struct HippoLoadAPI* load_api);
	void* priv;
} HippoPlaybackPlugin;

#define HIPPO_PLAYBACK_PLUGIN_API_VERSION 1
#define HIPPO_MESSAGE_API_VERSION 1

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

