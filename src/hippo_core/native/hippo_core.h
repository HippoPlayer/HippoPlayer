#pragma once

struct HippoCore;
struct HippoServiceAPI;

typedef struct HippoCore* (*HippoCoreNew)();
typedef void (*HippoCoreDrop)(struct HippoCore* core);
typedef void (*HippoPlayFile)(struct HippoCore* core, const char* filename);
typedef struct HippoServiceAPI* (*HippoServiceApiNew)(struct HippoCore* core);

extern HippoCoreNew hippo_core_new;
extern HippoCoreDrop hippo_core_drop;
extern HippoPlayFile hippo_play_file;
extern HippoServiceApiNew hippo_service_api_new;
