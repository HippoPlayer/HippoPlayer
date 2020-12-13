#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
extern "C"
{
#endif

#include "HippoSettings.h"

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
typedef int32_t HippoIoErrorCode;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoIoAPI {
	int (*exists)(struct HippoApiPrivData* priv_data, const char* filename);

	HippoIoErrorCode (*read_file_to_memory)(struct HippoApiPrivData* priv_data, const char* filename, void** dest, uint64_t* size);
	HippoIoErrorCode (*free_file_to_memory)(struct HippoApiPrivData* priv_data, void* dest);

    // Io functions for more control
	HippoIoErrorCode (*open)(struct HippoApiPrivData* priv_data, const char* target, HippoIoHandle* handle);
	HippoIoErrorCode (*close)(HippoIoHandle handle);
	HippoIoErrorCode (*size)(HippoIoHandle handle, uint64_t* res);
	HippoIoErrorCode (*read)(HippoIoHandle handle, void* dest, uint64_t size);
	HippoIoErrorCode (*seek)(HippoIoHandle handle, HippoFileSeek type, int64_t step);

	struct HippoApiPrivData* priv_data;
} HippoIoAPI;

#define HippoIo_read_file_to_memory(api, filename, dest, size) api->read_file_to_memory(api->priv_data, filename, dest, size)
#define HippoIo_free_file_to_memory(api, dest) api->free_file_to_memory(api->priv_data, dest)

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

typedef uint64_t HippoMetadataId;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HippoMetadata_TitleTag "title"
#define HippoMetadata_SongTypeTag "song_type"
#define HippoMetadata_AuthoringToolTag "authoring_tool"
#define HippoMetadata_ArtistTag "artist"
#define HippoMetadata_AlbumTag "album"
#define HippoMetadata_DateTag "date"
#define HippoMetadata_GenreTag "genre"
#define HippoMetadata_MessageTag "message"
#define HippoMetadata_LengthTag "length"
#define HippoMetadata_SamplesTag "sample_"
#define HippoMetadata_InstrumentsTag "instrument_"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoMetadataAPI {
	struct HippoMetadataAPIPrivData* priv_data;

    HippoMetadataId (*create_url)(
        struct HippoMetadataAPIPrivData* priv_data,
        const char* url);

    void (*set_tag)(
        struct HippoMetadataAPIPrivData* priv_data,
        HippoMetadataId id,
        const char* tag,
        const char* data);

    void (*set_tag_f64)(
        struct HippoMetadataAPIPrivData* priv_data,
        HippoMetadataId id,
        const char* tag,
        double d);

    void (*add_subsong)(
        struct HippoMetadataAPIPrivData* priv_data,
        HippoMetadataId parent_id,
        int index,
        const char* name,
        float length);

    void (*add_sample)(
        struct HippoMetadataAPIPrivData* priv_data,
        HippoMetadataId parent_id,
        const char* text);

    void (*add_instrument)(
        struct HippoMetadataAPIPrivData* priv_data,
        HippoMetadataId parent_id,
        const char* text);

    int (*begin_get_all)(
        struct HippoMetadataAPIPrivData* priv_data,
        const char* url);

    void (*end_get_all)(
        struct HippoMetadataAPIPrivData* priv_data);

    int (*get_all_entry)(
        struct HippoMetadataAPIPrivData* priv_data,
        int entry,
        const char** name,
        const char** data,
        int* len_name,
        int* len_data);

    int (*get_all_sample)(
        struct HippoMetadataAPIPrivData* priv_data,
        int entry,
        const char** text,
        int* text_len);

    int (*get_all_instrument)(
        struct HippoMetadataAPIPrivData* priv_data,
        int entry,
        const char** text,
        int* text_len);

} HippoMetadataAPI;

#define HIPPO_METADATA_API_VERSION 1

#define HippoMetadata_create_url(api, url) api->create_url(api->priv_data, url)
#define HippoMetadata_set_tag(api, id, tag, data) api->set_tag(api->priv_data, id, tag, data)
#define HippoMetadata_set_tag_f64(api, id, tag, data) api->set_tag_f64(api->priv_data, id, tag, data)

#define HippoMetadata_add_subsong(api, url, index, name, len) api->add_subsong(api->priv_data, url, index, name, len)
#define HippoMetadata_add_sample(api, url, text) api->add_sample(api->priv_data, url, text)
#define HippoMetadata_add_instrument(api, url, text) api->add_instrument(api->priv_data, url, text)

#define HippoMetadata_begin_get_all(api, url) api->begin_get_all(api->priv_data, url)
#define HippoMetadata_end_all(api) api->begin_get_all(api->priv_data)
#define HippoMetadata_get_all_entry(api, index, name, data, name_len, data_len) api->get_all_entry(api->priv_data, index, name, data, name_len, data_len)
#define HippoMetadata_get_all_sample(api, index, text, text_len) api->get_all_sample(api->priv_data, index, text, text_len)
#define HippoMetadata_get_all_instrument(api, index, text, text_len) api->get_all_instrument(api->priv_data, index, text, text_len)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum HippoSettingResult {
	HippoSettingsResult_Ok = 0,
	HippoSettingsResult_SettingNotFound = 1,
	HippoSettingsResult_KeyNotFound = 2,
	HippoSettingsResult_InvalidType = 3,
} HippoSettingResult;

struct HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plugins can use the MessageAPI to subscribe to events and post data that is being requested

typedef struct HippoMessageAPI {
    // Private internal data
	struct HippoMessageAPI* priv_data;
	// Send message (data must be a Flatbuffer formatted message)
	void (*send)(struct HippoMessageAPI* handle, const unsigned char* data, int len);

} HippoMessageAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HippoMessageAPI_send(api, data, len) api->send(api->priv_data, data, len)

enum { HIPPO_LOG_TRACE, HIPPO_LOG_DEBUG, HIPPO_LOG_INFO, HIPPO_LOG_WARN, HIPPO_LOG_ERROR, HIPPO_LOG_FATAL };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HIPPO_LOG_API_VERSION 1

typedef struct HippoLogAPI {
    void* priv_data;
    void (*log_set_base_name)(void* priv_data, const char* base_name);
    void (*log)(void* priv_data, int level, const char* file, int line, const char* fmt, ...);
} HippoLogAPI;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define hp_debug(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_DEBUG, NULL, 0, __VA_ARGS__); }

#define hpfl_debug(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__); }

#define hp_trace(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_TRACE, NULL, 0, __VA_ARGS__); }

#define hpfl_trace(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__); }

#define hp_info(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_INFO, NULL, 0, __VA_ARGS__); }

#define hpfl_info(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__); }

#define hp_warn(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_WARN, NULL, 0, __VA_ARGS__); }

#define hpfl_warn(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__); }

#define hp_error(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_ERROR, NULL, 0, __VA_ARGS__); }

#define hpfl_error(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__); }

#define hp_fatal(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_FATAL, NULL, 0, __VA_ARGS__); }

#define hpfl_fatal(...) { \
    extern HippoLogAPI* g_hp_log;  \
    g_hp_log->log(g_hp_log->priv_data, HIPPO_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoServicePrivData;
struct HippoSettingsAPI;

typedef struct HippoServiceAPI {
	void* private_data;
	const struct HippoLogAPI* (*get_log_api)(void* private_data, int api_version);
	const struct HippoIoAPI* (*get_io_api)(void* private_data, int api_version);
	const struct HippoMetadataAPI* (*get_metadata_api)(void* private_data, int api_version);
	const struct HippoMessageAPI* (*get_message_api)(void* private_data, int api_version);
	const struct HippoSettingsAPI* (*get_settings_api)(void* private_data, int api_version);
} HippoServiceAPI;

#define HippoServiceAPI_get_log_api(api, version) api->get_log_api(api->private_data, version)
#define HippoServiceAPI_get_io_api(api, version) api->get_io_api(api->private_data, version)
#define HippoServiceAPI_get_metadata_api(api, version) api->get_metadata_api(api->private_data, version)
#define HippoServiceAPI_get_message_api(api, version) api->get_message_api(api->private_data, version)
#define HippoServiceAPI_get_settings_api(api, version) api->get_settings_api(api->private_data, version)

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
struct HippoMetadataAPI;
struct HippoSettingsAPI;

typedef struct HippoPlaybackPlugin {
	uint64_t api_version;
	const char* name;
	const char* version;
	const char* library_version;
	enum HippoProbeResult (*probe_can_play)(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size);
	const char* (*supported_extensions)();
	void* (*create)(const HippoServiceAPI* services);
	int (*destroy)(void* user_data);
	void (*event)(void* user_data, const unsigned char* data, int len);
	int (*open)(void* user_data, const char* buffer, int subsong);
	int (*close)(void* user_data);
	int (*read_data)(void* user_data, void* dest, uint32_t max_sample_count);
	int (*seek)(void* user_data, int ms);
	int (*metadata)(const char* url, const HippoServiceAPI* services);
	void (*static_init)(struct HippoLogAPI* log, const HippoServiceAPI* services);
	int (*register_settings)(const struct HippoSettingsAPI* settings);
	int (*update_settings)(void* user_data, const struct HippoSettingsAPI* settings);
} HippoPlaybackPlugin;


#define HIPPO_PLAYBACK_PLUGIN_API_VERSION 1
#define HIPPO_MESSAGE_API_VERSION 1

//#define HIPPO_NAME(name) (u64)name
//#define HIPPO_VERSION(text) (u64)text

#define hippo_min(x, y) (((x) < (y)) ? (x) : (y))

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

