#pragma once

#include <stdbool.h>

#define HS_FLOAT_TYPE 0x1000
#define HS_INTEGER_TYPE 0x1001
#define HS_BOOL_TYPE 0x1002
#define HS_INTEGER_RANGE_TYPE 0x1003
#define HS_STRING_RANGE_TYPE 0x1004

#define HIPPO_SETTINGS_API_VERSION 1

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSBase {
    const char* widget_id;
    const char* name;
    const char* desc;
    int widget_type;
} HSBase;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSFloat {
    HSBase base;
    float value;
    float start_range;
    float end_range;
} HSFloat;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSInteger {
    HSBase base;
    int value;
    int start_range;
    int end_range;
} HSInteger;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSBool {
    HSBase base;
    bool value;
} HSBool;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSIntegerRangeValue {
    const char* name;
    int value;
} HSIntegerRangeValue;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSStringRangeValue {
    const char* name;
    const char* value;
} HSStringRangeValue;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// start of this layout has to match HSInteger

typedef struct HSIntegerFixedRange {
    HSBase base;
    int value;
    HSIntegerRangeValue* values;
    int values_count;
} HSIntegerFixedRange;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct StringFixedRange {
    HSBase base;
    const char* value;
    HSStringRangeValue* values;
    int values_count;
} HSStringFixedRange;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union HSSetting {
    HSInteger int_value;
    HSFloat float_value;
    HSIntegerFixedRange int_fixed_value;
    HSStringFixedRange string_fixed_value;
    HSBool bool_value;
} HSSetting;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define hp_sizeof_array(x) sizeof(x) / sizeof(x[0])
#define HSIntValue(id, name, desc, value)                             \
    {                                                                 \
        .int_value = { id, name, desc, HS_INTEGER_TYPE, value, 0, 0 } \
    }
#define HSFloatValue(id, name, desc, value)                                 \
    {                                                                       \
        .float_value = { id, name, desc, HS_FLOAT_TYPE, value, 0.0f, 0.0f } \
    }
#define HSFloatValue_Range(id, name, desc, value, start, end)               \
    {                                                                       \
        .float_value = { id, name, desc, HS_FLOAT_TYPE, value, start, end } \
    }
#define HSBoolValue(id, name, desc, value)                    \
    {                                                         \
        .bool_value = { id, name, desc, HS_BOOL_TYPE, value } \
    }
#define HSIntValue_Range(id, name, desc, value, min, max)                 \
    {                                                                     \
        .int_value = { id, name, desc, HS_INTEGER_TYPE, value, min, max } \
    }
#define HSIntValue_DescRange(id, name, desc, value, ranges) \
    {                                                       \
        .int_fixed_value = {                                \
            id,                                             \
            name,                                           \
            desc,                                           \
            HS_INTEGER_RANGE_TYPE,                          \
            value,                                          \
            (HSIntegerRangeValue*)&ranges,                  \
            hp_sizeof_array(ranges)                         \
        }                                                   \
    }
#define HSStringValue_DescRange(id, name, desc, value, ranges) \
    {                                                          \
        .string_fixed_value = {                                \
            id,                                                \
            name,                                              \
            desc,                                              \
            HS_STRING_RANGE_TYPE,                              \
            value,                                             \
            (HSStringRangeValue*)&ranges,                      \
            hp_sizeof_array(ranges)                            \
        }                                                      \
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum HippoSettingsError {
    HippoSettingsError_Ok,
    HippoSettingsError_NotFound,
    HippoSettingsError_DuplicatedId,
    HippoSettingsError_WrongType,
} HippoSettingsError;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoSettingsAPI {
    void* priv_data;

    // Register the settings to be used for the playback plugin
    HippoSettingsError (*register_settings)(void* priv_data, const char* name, const HSSetting* settings, int count);

    // This will be used to allow structring the layout a bit better
    // HippoSettingsError (*layout_hints)(void* priv_data, void* data);

    // access settings
    HippoSettingsError (*get_string)(void* priv_data, const char* ext, const char* id, char* value, int max_len);
    HippoSettingsError (*get_int)(void* priv_data, const char* ext, const char* id, int* value);
    HippoSettingsError (*get_float)(void* priv_data, const char* ext, const char* id, float* value);
    HippoSettingsError (*get_bool)(void* priv_data, const char* ext, const char* id, bool* value);

    // Update settings
    // HippoSettingError (*set_string)(void* priv_data, int id, char* value);
    // HippoSettingError (*set_int)(void* priv_data, int id, int* value);
    // HippoSettingError (*set_float)(void* priv_data, int id, int* value);

    // makes it possible to disable / enable a control
    // HippoSettingsError (*enable_ctl)(void* priv_data, int id, bool state);

    // get the last error (null if no error)
    // const char* (*get_last_error)(void* priv_data);

} HippoRegisterSettingsAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define HippoSettings_register_settings(api, name, settings) \
    api->register_settings(api->priv_data, name, (HSSetting*)&settings, hp_sizeof_array(settings))

#define HippoSettings_get_string(api, id, value, max_len) \
    api->get_string(api->priv_data, id, value, max_len)

#define HippoSettings_get_int(api, id, value) \
    api->get_int(api->priv_data, id, value)

#define HippoSettings_get_float(api, id, value) \
    api->get_float(api->priv_data, id, value)

#define HippoSettings_get_bool(api, id, value) \
    api->get_bool(api->priv_data, id, value)



// #define HippoSettings_get_last_error(api) api->get_last_error(api->priv_data)
