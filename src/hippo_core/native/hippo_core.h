#pragma once

struct SongDb;

typedef struct SongDb* (*HippoSongDbNew)();
typedef void (*HippoSongDbUpdate)(struct SongDb* db, const char* name);

extern HippoSongDbNew hippo_song_db_new;
extern HippoSongDbUpdate hippo_song_db_update;
