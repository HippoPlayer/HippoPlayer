#pragma once

#include <stdbool.h>

#define HS_FLOAT_TYPE 0x1000
#define HS_INTEGER_TYPE 0x1001
#define HS_BOOL_TYPE 0x1002
#define HS_INTEGER_RANGE_TYPE 0x1003
#define HS_STRING_RANGE_TYPE 0x1003

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSBase {
	int widget_type;
	int widget_id;
	const char* name;
	const char* desc;
} HSBase;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSFloat {
	HSBase base;
	float start_value;
	float start_range;
	float end_range;
} HSFloat;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HSInteger {
	HSBase base;
	int start_value;
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

#define hp_sizeof_array(x) sizeof(x) / sizeof(x [0])
#define HSIntValue(id, name, desc, value) { .int_value = { HU_INTEGER_TYPE, id, name, desc, value, -1, ~0 } }
#define HSFloatValue(id, name, desc, value) { .float_value = { HU_FLOAT_TYPE, id, name, desc, value, 0.0f, 10.0f } }
#define HSBoolValue(id, name, desc, value) { .bool_value = { HU_BOOL_TYPE, id, name, desc, value } }
#define HSIntValue_Range(id, name, desc, value, min, max) { .int_value = { HU_INTEGER_TYPE, id, name, desc, value, min, max } }
#define HSIntValue_DescRange(id, name, desc, value, ranges) { .int_fixed_value = { HU_INTEGER_RANGE_TYPE, id, name, desc, value, (HUIntegerRangeValue*)&ranges, hp_sizeof_array(ranges) } }
#define HSStringValue_DescRange(id, name, desc, value, ranges) { .string_fixed_value = { HU_STRING_RANGE_TYPE, id, name, desc, value, (HUStringRangeValue*)&ranges, hp_sizeof_array(ranges) } }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum HippoSettingError {
    HippoSettingError_Ok,
    HippoSettingError_NotFound,
    HippoSettingError_DuplicatedId,
    HippoSettingError_WrongType,
} HippoSettingError;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoSettingsAPI {
    void* priv_data;
    HippoSettingError (*register_filetype_settings)(void* priv_data, const HSSetting* settings, int count, const char* error);
    HippoSettingError (*register_global_settings)(void* priv_data, const HSSetting* settings, int count, const char* error);

    // access settings
    HippoSettingError (*get_string)(void* priv_data, int id, char* value);
    HippoSettingError (*get_int)(void* priv_data, int id, int* value);
    HippoSettingError (*get_float)(void* priv_data, int id, int* value);

    // Update settings
    HippoSettingError (*set_string)(void* priv_data, int id, char* value);
    HippoSettingError (*set_int)(void* priv_data, int id, int* value);
    HippoSettingError (*set_float)(void* priv_data, int id, int* value);

    // makes it possible to disable / enable a control
    HippoSettingError (*enable_ctl)(void* priv_data, int id, bool state);

} HippoRegisterSettingsAPI;





