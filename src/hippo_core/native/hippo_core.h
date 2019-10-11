#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct HippoCore;
struct HippoServiceAPI;
struct HippoMessageAPI;

typedef const struct HippoMessageAPI* (*HippoGetMessages)(void* user_data, int index);
typedef void (*HippoSendMessage)(void* user_data, const unsigned char* data, int len, int index);

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
typedef void (*HippoPlaylistRemoveEntry)(struct HippoCore* core, int playlist_entry);

extern HippoCoreNew hippo_core_new;
extern HippoCoreDrop hippo_core_drop;
extern HippoPlayFile hippo_play_file;
extern HippoServiceApiNew hippo_service_api_new;
extern HippoUpdateMessages hippo_update_messages;
extern HippoPlaylistRemoveEntry hippo_playlist_remove_entry;

#ifdef __cplusplus
}
#endif

