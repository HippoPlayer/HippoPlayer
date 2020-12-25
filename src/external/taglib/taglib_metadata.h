#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HippoMetadataAPI;

uint64_t taglib_update_metadata(const char* url, const struct HippoMetadataAPI* api);

#ifdef __cplusplus
}
#endif

