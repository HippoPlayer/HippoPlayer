#pragma once

#define HU_FLOAT_TYPE 0x1000
#define HU_INTEGER_TYPE 0x1001
#define HU_BOOL_TYPE 0x1002
#define HU_INTEGER_RANGE_TYPE 0x1003
#define HU_STRING_RANGE_TYPE 0x1003

typedef struct HUBase {
	int widget_type;
	int widget_id;
	const char* name;
	const char* desc;
} HUBase;

typedef struct HUFloat {
	HUBase base;
	float start_value;
	float start_range;
	float end_range;
} HUFloat;

typedef struct Integer {
	HUBase base;
	int start_value;
	int start_range;
	int end_range;
} HUInteger;

typedef struct HUBool {
	HUBase base;
	bool value;
} HUBool;

typedef struct HUIntegerRangeValue {
	const char* name;
	int value;
} HUIntegerRangeValue;

typedef struct HUStringRangeValue {
	const char* name;
	const char* value;
} HUStringRangeValue;

typedef struct HUIntegerFixedRange {
	HUBase base;
	int value;
	HUIntegerRangeValue* values;
	int values_count;
} HUIntegerFixedRange;

typedef struct StringFixedRange {
	HUBase base;
	const char* value;
	HUStringRangeValue* values;
	int values_count;
} HUStringFixedRange;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union HUWidget {
	HUInteger int_value;
	HUFloat float_value;
	HUIntegerFixedRange int_fixed_value;
	HUStringFixedRange string_fixed_value;
	HUBool bool_value;
} HUWidget;

