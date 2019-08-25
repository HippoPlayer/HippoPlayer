#pragma once

struct HippoCore;

typedef struct HippoCore* (*HippoCoreNew)();
typedef void (*HippoCoreDrop)(struct HippoCore* core);

extern HippoCoreNew hippo_core_new;
extern HippoCoreDrop hippo_core_drop;
