extern crate hippo_api;
extern crate messages;
extern crate rodio;
extern crate dynamic_reload;
extern crate walkdir;

#[macro_use]
extern crate serde_derive;
pub extern crate rmp;
extern crate rmp_rpc;
extern crate serde;
extern crate serde_json;

extern crate rmp_serde as rmps;

pub mod audio;
pub mod playlist;
pub mod plugin_handler;
pub mod song_db;
pub mod service;
pub mod service_ffi;
pub mod decoder_plugins;

pub use audio::*;
pub use plugin_handler::*;
pub use playlist::*;
pub use song_db::*;
pub use service::*;
pub use service_ffi::*;
pub use decoder_plugins::*;

use std::fs;
use std::fs::File;
use std::ffi::{CString, CStr};
use std::io::Read;

pub struct HippoCore<'a> {
    audio: HippoAudio,
    decoder_plugins: DecoderPlugins<'a>,
    plugin_service: service_ffi::PluginService,
}


impl<'a> HippoCore<'a> {
    pub fn new() -> HippoCore<'a>  {
        HippoCore {
            audio: HippoAudio::new(),
            decoder_plugins: DecoderPlugins::new(),
            plugin_service: service_ffi::PluginService::new(),
        }
    }

    ///
    /// This will ask a plugin if it can play a file or not. Notice that several plugins may be
    /// able to play the same format
    ///
    fn probe_can_play(plugin: &DecoderPlugin, data: &[u8], buffer_len: usize, filename: &str, file_size: u64) -> bool {
        let c_filename = CString::new(filename).unwrap();
        let res = ((plugin.plugin_funcs).probe_can_play)(data.as_ptr(), buffer_len as u32, c_filename.as_ptr(), file_size);

        match res {
            0 => true,
            _ => false,
        }
    }

    ///
    /// Play a a file located on a local readable device
    ///
    pub fn play_file(&mut self, filename: &str) -> MusicInfo {
        let mut buffer = [0; 4096];
        // TODO: Fix error handling here

        let metadata = fs::metadata(filename).unwrap();
        let mut f = File::open(filename).unwrap();
        let buffer_read_size = f.read(&mut buffer[..]).unwrap();

        // TODO: Proper priority order of plugins. Right now we just put uade as the last one to
        // check

        // find a plugin that supports the file

        for plugin in &self.decoder_plugins.handler.plugins {
            if plugin.plugin_path.rfind("uade").is_some() {
                continue;
            }

            if Self::probe_can_play(&plugin, &buffer, buffer_read_size, filename, metadata.len()) {
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

        for plugin in &self.decoder_plugins.handler.plugins {
            if plugin.plugin_path.rfind("uade").is_none() {
                continue;
            }

            if Self::probe_can_play(&plugin, &buffer, buffer_read_size, filename, metadata.len()) {
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


