use std::collections::HashMap;
use std::ffi::CStr;
use std::fs;
use std::fs::File;
use std::os::raw::{c_char, c_void};
use std::path::Path;
use std::ptr;

mod audio;
mod core_config;
mod playlist;
mod plugin_handler;
mod song_db;

pub mod ffi;
mod service;
pub mod service_ffi;

use audio::{HippoAudio, MusicInfo};
use core_config::CoreConfig;
use messages::*;
use playlist::Playlist;
use plugin_handler::Plugins;
use service_ffi::{PluginService, ServiceApi};

use std::io::Read;

type MsgGetCallback =
    extern "C" fn(user_data: *const c_void, index: u32) -> *const ffi::HippoMessageAPI;
type MsgSendCallback =
    extern "C" fn(user_data: *const c_void, data: *const u8, len: i32, index: i32);

#[derive(Default, Debug)]
pub struct SongDb {
    data: HashMap<String, String>,
}

pub struct HippoCore {
    config: CoreConfig,
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

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.probe_can_play(&buffer, buffer_read_size, filename, metadata.len()) {
                if self
                    .plugin_service
                    .get_song_db()
                    .get_data(filename)
                    .is_none()
                {
                    plugin.get_metadata(&filename, &self.plugin_service);
                }

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

        println!("Unable to find plugin to support {}", filename);

        MusicInfo::default()
    }

    ///
    /// Post messages to the frontend and make sure to not send to self
    ///
    fn send_msgs(
        user_data: *const c_void,
        send_messages: MsgSendCallback,
        data: &[u8],
        count: usize,
        current_index: usize,
    ) {
        for msg_index in (0..count).filter(|v| *v != current_index) {
            send_messages(
                user_data,
                data.as_ptr(),
                data.len() as i32,
                msg_index as i32,
            );
        }
    }

    fn update_messages(
        &mut self,
        user_data: *const c_void,
        count: u32,
        get_messages: MsgGetCallback,
        send_messages: MsgSendCallback,
    ) {
        let count = count as usize;

        // Send the UI messages to playlist, each-other and to the backends

        for msg_index in 0..count {
            let msgs = ServiceApi::get_message_api_from_c_api_mut(get_messages(
                user_data,
                msg_index as u32,
            ));
            let msg_count = msgs.read_stream.len();

            for _ in 0..msg_count {
                let message_data = msgs.read_stream.pop().unwrap();
                let message = get_root_as_hippo_message(&message_data);

                // Send messages to the frontend
                Self::send_msgs(user_data, send_messages, &message_data, count, msg_index);

                // Send the message to to playlist and
                self.playlist.event(&message).map(|reply| {
                    Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
                });

                // If we have active playbacks we push the ui messages to the messages plugin
                // instance so the backend can reply to them at some point

                for playback in &mut self.audio.playbacks {
                    playback
                        .write_stream
                        .push(message_data.to_vec().into_boxed_slice())
                        .unwrap();
                }
            }
        }

        // If current song has been updated with messages above we try starting playing the new one

        if self.playlist.is_current_song_updated() {
            let new_song = self.playlist.get_current_song();

            if let Some(song) = new_song {
                self.play_file(&song);

                if let Some(metadata) = self.plugin_service.get_song_db().get_data(&song) {
                    let message = get_root_as_hippo_message(&metadata);
                    let metadata_message = message.message_as_song_metadata().unwrap();
                    self.playlist.update_current_entry(&metadata_message);

                    self.playlist.current_song_message().map(|reply| {
                        Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
                    });
                }
            }
        }

        // send the messages from the backends to the UI

        let backend_messages = self.plugin_service.get_message_api_mut();
        let msg_count = backend_messages.read_stream.len();

        for _ in 0..msg_count {
            let message_data = backend_messages.read_stream.pop().unwrap();
            Self::send_msgs(
                user_data,
                send_messages,
                &message_data,
                count,
                std::usize::MAX,
            );
        }
    }
}

/// Create new instance of the song db
#[no_mangle]
pub extern "C" fn hippo_core_new() -> *const HippoCore {
    let config = match CoreConfig::load("data/config/global.cfg") {
        Ok(v) => v,
        Err(e) => {
            println!("Config error: {}", e);
            return ptr::null();
        }
    };

    let mut plugins = Plugins::new();

    let current_path = std::env::current_dir().unwrap();

    // TODO: We should do better error handling here
    // This to enforce we load relative to the current exe
    let current_exe = std::env::current_exe().unwrap();
    std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();
    let path = std::env::current_dir().unwrap();

    println!("The current directory is {}", path.display());

    plugins.add_plugins_from_path();

    // sort plugins according to priority order

    let plugin_count = plugins.decoder_plugins.len();

    // sort high-prority plugins
    for (index, name) in config.playback_priority_high.iter().enumerate() {
        for i in 0..plugin_count {
            let lower_name = plugins.decoder_plugins[i].plugin_path.to_lowercase();
            if lower_name.rfind(name).is_some() && plugin_count > index {
                plugins.decoder_plugins.swap(index, i);
            }
        }
    }

    // sort low-prority plugins
    for (index, name) in config.playback_priority_low.iter().enumerate() {
        for i in 0..plugin_count {
            let lower_name = plugins.decoder_plugins[i].plugin_path.to_lowercase();
            if lower_name.rfind(name).is_some() && plugin_count > index {
                plugins.decoder_plugins.swap((plugin_count - 1) - index, i);
            }
        }
    }

    let mut core = Box::new(HippoCore {
        config,
        plugins,
        audio: HippoAudio::new(),
        plugin_service: service_ffi::PluginService::new(),
        playlist: Playlist::new(),
        _current_song_time: -10.0,
        _is_playing: false,
    });

    // No need to switch back if we are in the correct spot
    if !Path::new("bin").is_dir() {
        std::env::set_current_dir(current_path).unwrap();
    }

    // it's ok to allow this function to fail if we have no playlist
    core.playlist.load("default_playlist.hpl").ok();

    Box::into_raw(core) as *const HippoCore
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_core_drop(core: *mut HippoCore) {
    let mut core = unsafe { Box::from_raw(core) };

    core.audio.stop();
    core.playlist
        .save("default_playlist.hpl")
        .unwrap_or_else(|err| {
            println!("Unable to save default playlist {}", err);
        });

    // core will be dropped at this point
}

/// Update the song db with a new entry
#[no_mangle]
pub unsafe extern "C" fn hippo_play_file(_core: *mut HippoCore, filename: *const c_char) {
    let core = &mut *_core;
    let slice = CStr::from_ptr(filename);

    core.play_file(slice.to_str().unwrap());
    //println!("filename to play returned: {}", slice.to_str().unwrap());
}

#[no_mangle]
pub extern "C" fn hippo_service_api_new() -> *const ffi::HippoServiceAPI {
    PluginService::new_c_api()
}

#[no_mangle]
pub extern "C" fn hippo_message_api_new() -> *const ffi::HippoMessageAPI {
    service_ffi::new_c_message_api()
}

#[no_mangle]
pub unsafe extern "C" fn hippo_update_messages(
    core: *mut HippoCore,
    user_data: *const c_void,
    count: u32,
    get_messages: MsgGetCallback,
    send_messages: MsgSendCallback,
) {
    let core = &mut *core;
    core.update_messages(user_data, count, get_messages, send_messages);
}

#[no_mangle]
pub unsafe extern "C" fn hippo_playlist_remove_entry(core: *mut HippoCore, entry: i32) {
    let core = &mut *core;
    core.playlist.remove_entry(entry);
}
