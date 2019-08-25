use std::collections::HashMap;
use std::os::raw::c_char;

/*
use hippo_api;
use messages;
use rodio;
use walkdir;

#[macro_use]
use serde_derive;
pub use rmp;
use rmp_rpc;
use serde;
use serde_json;
*/

mod audio;
mod playlist;
mod plugin_handler;
mod song_db;

mod service;
pub mod service_ffi;

use audio::HippoAudio;
use plugin_handler::Plugins;
use playlist::Playlist;

pub use rmp as msgpack;

#[derive(Default, Debug)]
pub struct SongDb {
    data: HashMap<String, String>,
}

pub struct HippoCore<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    plugin_service: service_ffi::PluginService,
    playlist: Playlist,
    _current_song_time: f32,
    _is_playing: bool,
}

/// Create new instance of the song db
#[no_mangle]
pub extern "C" fn hippo_core_new() -> *const HippoCore<'static> {
    let mut core = Box::new(HippoCore {
        audio: HippoAudio::new(),
        plugins: Plugins::new(),
        plugin_service: service_ffi::PluginService::new(),
        playlist: Playlist::new(),
        _current_song_time: -10.0,
        _is_playing: false,
    });

    core.plugins.add_plugins_from_path();

    Box::into_raw(core) as *const HippoCore
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_core_drop(_db: *mut SongDb, _name: *const c_char) {
    /*
    let db = unsafe { &mut *_db };
    println!("{:?}", db);
    */
}
