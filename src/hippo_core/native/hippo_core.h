#pragma once

struct HippoCore;
struct HippoServiceAPI;
struct HippoMessageAPI;
struct HippoMessageDecode;

typedef const struct HippoMessageAPI* (*HippoGetMessages)(void* user_data, int index);
typedef void (*HippoSendMessage)(void* user_data, const struct HippoMessageDecode* msg, int index);

typedef struct HippoCore* (*HippoCoreNew)();
typedef void (*HippoCoreDrop)(struct HippoCore* core);
typedef void (*HippoPlayFile)(struct HippoCore* core, const char* filename);
typedef struct HippoServiceAPI* (*HippoServiceApiNew)(struct HippoCore* core);
typedef struct HippoMessageAPI* (*HippoMessageApiNew)(struct HippoCore* core);

typedef void (*HippoUpdateMessages)(struct HippoCore* core,
                                    void* user_data,
                                    int count,
                                    HippoGetMessages get_messages,
                                    HippoSendMessage send_messages);

extern HippoCoreNew hippo_core_new;
extern HippoCoreDrop hippo_core_drop;
extern HippoPlayFile hippo_play_file;
extern HippoServiceApiNew hippo_service_api_new;
extern HippoUpdateMessages hippo_update_messages;