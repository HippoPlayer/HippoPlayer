#pragma once

struct HippoCore;

typedef struct HippoCore* (*HippoCoreNew)();
typedef void (*HippoCoreDrop)(struct HippoCore* core);
typedef void (*HippoPlayFile)(struct HippoCore* core, const char* filename);

extern HippoCoreNew hippo_core_new;
extern HippoCoreDrop hippo_core_drop;
extern HippoPlayFile hippo_play_file;
