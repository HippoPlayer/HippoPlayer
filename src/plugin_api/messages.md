## Message API description

HippoPlayer uses [msgpack](https://msgpack.org) for internal and external commination. The idea is the external programs can both listen in and send commands to HippoPlayer. Also while this adds a slightly overhead for plugins internally with in the program the flexibility and the extensibility of the feature makes up for it. In general the overhead should be petty small as it's mainly mem-copies internally in the program and should hopefully stay in cache most of the time.
When sending commands to HippoPlayer the [msgpack-rpc format](https://github.com/msgpack-rpc/msgpack-rpc/blob/master/spec.md) is being used. All commands bellow are described without the header and only the type of request/notification is noted. It's expected that external tools sending data to HippoPlayer follows the standard or otherwise the commands will not work as expected.

Notice in this doc that **URL** is used for all kind of filenames (local,remote,weburls,etc) as they are the same in the view of hippoplayer

*TODO* Description of compresssed files playback (not supported yet but perhaps something like foo.zip:dir/song.mod)

### hippo_metadata_update_entry (request)

This is used to updated metadata about an resource (filename,url,etc) all fields for each sub song is optional

```
  [resource]  - string (filename/url/etc)
  [array_len] - integer (sub song count, set this to 1 if there is only one song]
    for each sub song:
      [array_len]      - int    : Number of fields
      [title]          - string : Name of the song
      [type]           - string : Type of song (for example mod)
      [authoring_tool] - string : Tool used for creating the music
      [date]           - string : Creation date of the music
      [message]        - string : Message
      [samples]        - string : Write this if the song contains samples
        [array_len]    - int    : Number of samples
        [strings..]    - string : List of strings (needs to match written array length)
      [instruments]    - string : Write this if the song contains instruments
        [array_len]    - int    : Number of samples
        [strings..]    - string : List of strings (needs to match written array length)
```

## hippo_playlist_loaded (notification)

New playlist has been loaded

```
  [array_len]   - int : Number for urls/files
  [strings..]   - string : List of strings to be added to the playlist
```

## hippo_playlist_add_urls (request)

Add files at the end of the playlist

```
  [array_len]   - int : Number for urls/files
  [strings..]   - string : List of strings to be added to the playlist
```

## hippo_playlist_add_files (request)

Reply that files actually got added with list of added files

```
  [array_len]   - int : Number for urls/files
  [strings..]   - string : List of strings to be added to the playlist
```

## hippo_playlist_select_song (notifcation)

Selects a song from the playlist and starts playing

```
  [index]   - int (The entry in the playlist, this is used as an opimization so that entry is tried first when selecting a track and it must match the name, otherwise the name is searched for)
                  If the entry wasn't found nothing is changed and if something is aleardy playing it will go on doing so.
  [url]     - string (filename/url) name of the url to play
```

## hippo_playlist_next_song (notifcation)

Selects the next song in the playlist (will loop around to first entry if user has settings to allow so)

## hippo_playlist_prev_song (notifcation)

Selects the previous song in the playlist (will loop around to first entry if user has settings to allow so)


