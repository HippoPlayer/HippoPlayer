use std::collections::HashMap;
use std::os::raw::c_char;
use std::path::Path;
use std::ffi::CStr;
use std::fs;
use std::fs::File;

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

use audio::{HippoAudio, MusicInfo};
use plugin_handler::Plugins;
use playlist::Playlist;
use service_ffi::{PluginService};
use ffi;

use std::io::Read;

#[derive(Default, Debug)]
pub struct SongDb {
    data: HashMap<String, String>,
}

pub struct HippoCore {
    audio: HippoAudio,
    plugins: Plugins,
    plugin_service: service_ffi::PluginService,
    playlist: Playlist,
    _current_song_time: f32,
    _is_playing: bool,
}

impl HippoCore {
    pub fn play_file(&mut self, filename: &str) -> MusicInfo {
        let mut buffer = [0; 4096];
        // TODO: Fix error handling here

        let metadata = fs::metadata(filename).unwrap();
        let mut f = File::open(filename).unwrap();
        let buffer_read_size = f.read(&mut buffer[..]).unwrap();

        // TODO: Proper priority order of plugins. Right now we just put uade as the last one to
        // check

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.plugin_path.rfind("uade").is_some() {
                continue;
            }

            if plugin.probe_can_play(&buffer, buffer_read_size, filename, metadata.len()) {
                // This is a bit hacky right now but will do the trick
                self.audio.stop();
                self.audio = HippoAudio::new();
                let info = self
                    .audio
                    .start_with_file(&plugin, &self.plugin_service, filename);

                //self.song_info_view.update_data(filename, &self.plugin_service.get_song_db());
                return info;
            }
        }

        // do the same fo uade

        for plugin in &self.plugins.decoder_plugins {
            if plugin.plugin_path.rfind("uade").is_none() {
                continue;
            }

            if plugin.probe_can_play(&buffer, buffer_read_size, filename, metadata.len()) {
                // This is a bit hacky right now but will do the trick
                self.audio.stop();
                self.audio = HippoAudio::new();
                let info = self
                    .audio
                    .start_with_file(&plugin, &self.plugin_service, filename);
                return info;
            }
        }

        println!("Unable to find plugin to support {}", filename);

        MusicInfo::default()
    }
}

/// Create new instance of the song db
#[no_mangle]
pub extern "C" fn hippo_core_new() -> *const HippoCore {
    let mut core = Box::new(HippoCore {
        audio: HippoAudio::new(),
        plugins: Plugins::new(),
        plugin_service: service_ffi::PluginService::new(),
        playlist: Playlist::new(),
        _current_song_time: -10.0,
        _is_playing: false,
    });

    let current_path = std::env::current_dir().unwrap();

    // TODO: We should do better error handling here
    // This to enforce we load relative to the current exe
    let current_exe = std::env::current_exe().unwrap();
    std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();
    let path = std::env::current_dir().unwrap();

    println!("The current directory is {}", path.display());

    core.plugins.add_plugins_from_path();

    // No need to switch back if we are in the correct spot
    if !Path::new("bin").is_dir() {
        std::env::set_current_dir(current_path).unwrap();
    }

    Box::into_raw(core) as *const HippoCore
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_core_drop(_core: *mut HippoCore) {
    // TODO: convert back to Box so it will be dropped
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_play_file(_core: *mut HippoCore, filename: *const c_char) {
    let core = unsafe { &mut *_core };
    let slice = unsafe { CStr::from_ptr(filename) };

    core.play_file(slice.to_str().unwrap());
    //println!("filename to play returned: {}", slice.to_str().unwrap());
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_service_api_new() -> *const ffi::HippoServiceAPI {
    PluginService::new_c_api()
}
