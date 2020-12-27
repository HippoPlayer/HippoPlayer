use anyhow::*;
use logger::*;
use song_db::SongDb;
use std::ffi::CString;
use std::fs;
use std::fs::File;
use std::os::raw::{c_char, c_void};
use std::path::{Path, PathBuf};
use std::time::Instant;

mod audio;
mod core_config;
mod playlist;
mod plugin_handler;

pub mod ffi;
mod service;
pub mod service_ffi;

use audio::HippoAudio;
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

pub struct HippoCore {
    //config_filename: String,
    error_string: Option<CString>,
    config: CoreConfig,
    audio: HippoAudio,
    plugins: Plugins,
    plugin_service: service_ffi::PluginService,
    playlist: Playlist,
    song_db: *const SongDb,
    current_song_time: f32,
    start_time: Instant,
    is_playing: bool,
}

impl HippoCore {
    /// Play a file. Url is the thing to play (often a filename) and playlist_index is
    /// is the index into the playlist. This is needed in case we want to add more songs to the playlist
    /// (such as subsongs) and the playlist doesn't have to search for the file
    pub fn play_file(&mut self, url: &str, playlist_index: usize) -> Result<String> {
        let mut buffer = [0; 32768];
        // TODO: Use Cow
        let url_no_sub;

        if let Some(separator) = url.find('|') {
            url_no_sub = url[..separator].to_owned();
        } else {
            url_no_sub = url.to_owned();
        }

        // TODO: Fix error handling here
        // TODO: cleanup
        // TODO: Store the player to use for subsongs as we know it already?

        let metadata = fs::metadata(&url_no_sub)?;
        let mut f = File::open(&url_no_sub)?;
        let buffer_read_size = f.read(&mut buffer[..])?;
        let song_db = self.plugin_service.get_song_db();

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.probe_can_play(&buffer, buffer_read_size, &url_no_sub, metadata.len()) {
                // we need stop before before doing metadata check because of UADE threads :(
                self.audio.stop();

                if !song_db.is_present(url) {
                    song_db.begin_transaction();
                    plugin.get_metadata(&url_no_sub, &self.plugin_service);
                    song_db.commit();
                }

                self.is_playing = true;

                // TODO: Config if this should be expanded async or not
                // TODO: Move to somewhere else?
                if let Some(subsongs) = song_db.get_subsongs(url) {
                    // if we have sub-songs we will add them here and also change
                    // the current file to be a "subsong (the first one)"
                    self.playlist
                        .insert_subsongs(playlist_index, &subsongs, url);
                    // add |0 at the end of url to mark it as a subsong
                    let sub_url = format!("{}|{}", url, subsongs[0].0);
                    self.audio
                        .start_with_file(&plugin, &self.plugin_service, &sub_url);
                    return Ok(sub_url);
                } else {
                    if self
                        .audio
                        .start_with_file(&plugin, &self.plugin_service, url)
                    {
                        return Ok(url.to_owned());
                    }
                }
            }
        }

        warn!("No playback plugin found for: {}", url);

        Ok(String::new())
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

        if self.is_playing {
            let current_time = Instant::now();
            let delta = (current_time - self.start_time).as_secs_f32();
            self.start_time = current_time;
            self.current_song_time -= delta;

            if self.current_song_time <= 0.0 {
                self.playlist.next_song();
            }
        }

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

                // send message to logging system
                logger::incoming_message(&message);

                // Send the message to to playlist and back to the ui
                self.playlist.event(&message).map(|reply| {
                    Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
                });

                // Send the message to to audio (to configure output, get devices, etc)
                self.audio.event(&message).map(|reply| {
                    Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
                });

                // Check if we should stop the current
                match message.message_type() {
                    MessageType::stop_song => {
                        self.is_playing = false;
                        self.audio.stop();
                    }
                    _ => (),
                }

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
                let song_name = self.play_file(&song.0, song.1);

                if let Ok(songname) = &song_name {
                    self.current_song_time = 5.0 * 60.0;

                    let song_db = self.plugin_service.get_song_db();

                    if self.plugin_service.get_song_db().is_present(&song.0) {
                        self.current_song_time =
                            song_db.get_tag_f64("length", &songname).unwrap() as f32;
                        self.playlist.update_current_entry(song_db, &songname);
                        self.start_time = Instant::now();

                        // TODO: Use setting
                        if self.current_song_time == 0.0 {
                            self.current_song_time = 5.0 * 60.0;
                        }

                        // Send playlist reply to frontend
                        self.playlist.current_song_message().map(|reply| {
                            Self::send_msgs(
                                user_data,
                                send_messages,
                                &reply,
                                count,
                                std::usize::MAX,
                            );
                        });
                    }
                }

                if let Err(e) = song_name {
                    warn!("Unable to play file {}", e);
                }
            }
        }

        // Send log messages (if enabled) to the front-end
        logger::send_log_messages().map(|reply| {
            Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
        });

        // if new subsongs has been inserted we need to send it to the frontend
        self.playlist.inserted_subsongs().map(|reply| {
            Self::send_msgs(user_data, send_messages, &reply, count, std::usize::MAX);
        });

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

    /// Finds the data directory relative to the executable.
    /// This is because it's possible to have data next to the exe, but also running
    /// the applications as t2-output/path/exe and the location is in the root then
    fn find_data_directory() -> Result<()> {
        let current_path =
            std::env::current_dir().map_err(|_| anyhow!("Unable to get current dir!"))?;
        if current_path.join("data").exists() {
            return Ok(());
        }

        let mut path = current_path
            .parent()
            .ok_or_else(|| anyhow!("Unable to get parent dir"))?;

        loop {
            trace!("seaching for data in {:?}", path);

            if path.join("data").exists() {
                std::env::set_current_dir(path)?;
                return Ok(());
            }

            path = path
                .parent()
                .ok_or_else(|| anyhow!("Unable to get parent dir"))?;
        }
    }

    /// Try to initalize the audio device, if it fails a C style error string will be returned here so
    /// the front-end can show it. As we do this early we don't have any message passing setup so it
    /// makes sense to have this specific
    fn init_audio_device(&mut self) -> Result<(), miniaudio::Error> {
        let device = self.config.audio_device.to_owned();
        self.audio.init_device(&device)
    }
}

/// init the data directory used for configs, logs, databases, etec
fn init_config_dir() -> Result<PathBuf> {
    if let Some(config_dir) = dirs::config_dir() {
        let dir = config_dir.join("HippoPlayer");
        if std::fs::create_dir_all(&dir).is_ok() {
            return Ok(dir);
        }
    }

    error!("Unable to create data directory for user");
    Err(anyhow!("Unable to init output dir for genertaed files!"))
}

#[no_mangle]
pub extern "C" fn hippo_core_new() -> *const HippoCore {
    let config_dir = init_config_dir();
    let mut config = CoreConfig::default();

    if let Ok(output_dir) = config_dir {
        logger::init_file_log(&output_dir);
        if let Ok(cfg) = CoreConfig::load(&output_dir, "global.cfg") {
            config = cfg;
        }
    }

    // TODO: We should do better error handling here
    // This to enforce we load relative to the current exe
    // TODO: clean this up
    let current_exe = std::env::current_exe().unwrap();
    std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();

    HippoCore::find_data_directory().expect("Unable to find data directory");
    let current_path = std::env::current_dir().unwrap();

    // TODO: We should do better error handling here
    // This to enforce we load relative to the current exe
    let current_exe = std::env::current_exe().unwrap();
    std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();

    let mut plugins = Plugins::new();

    plugins.add_plugins_from_path();

    // sort plugins according to priority order

    let plugin_count = plugins.decoder_plugins.len();

    // if we have no
    // this is somewhat temporary but should be fine for now, we split the configure in two parts
    // and the first entries are th high priority ones and the others the low prio

    let len = config.playback_priority.len();
    let high_prio = &config.playback_priority[0..len / 2];
    let low_prio = &config.playback_priority[len / 2..];

    // sort high-prority plugins
    for (index, name) in high_prio.iter().enumerate() {
        for i in 0..plugin_count {
            let lower_name = plugins.decoder_plugins[i].plugin_path.to_lowercase();
            if lower_name.rfind(name).is_some() && plugin_count > index {
                plugins.decoder_plugins.swap(index, i);
            }
        }
    }

    // sort low-prority plugins
    for (index, name) in low_prio.iter().enumerate() {
        for i in 0..plugin_count {
            let lower_name = plugins.decoder_plugins[i].plugin_path.to_lowercase();
            if lower_name.rfind(name).is_some() && plugin_count > index {
                plugins.decoder_plugins.swap((plugin_count - 1) - index, i);
            }
        }
    }

    // No need to switch back if we are in the correct spot
    if !Path::new("bin").is_dir() {
        std::env::set_current_dir(current_path).unwrap();
    }

    let song_db = Box::into_raw(Box::new(SongDb::new("songdb.db").unwrap()));
    let service = service_ffi::PluginService::new(song_db);

    let mut core = Box::new(HippoCore {
        error_string: None,
        config: config,
        plugins,
        audio: HippoAudio::new(),
        plugin_service: service,
        playlist: Playlist::new(),
        current_song_time: 0.0,
        start_time: Instant::now(),
        is_playing: false,
        song_db,
    });

    core.audio.device_name = core.config.audio_device.to_owned();

    if let Err(e) = core.audio.init_devices() {
        error!("Failed to find audio devices {:#?}", e);
    }

    // it's ok to allow this function to fail if we have no playlist
    core.playlist.load("default_playlist.hpl").ok();

    Box::into_raw(core) as *const HippoCore
}

/// Update the song db with a new entry
#[no_mangle]
pub extern "C" fn hippo_core_drop(core: *mut HippoCore) {
    let mut core = unsafe { Box::from_raw(core) };
    let _ = unsafe { Box::from_raw(core.song_db as *mut SongDb) };

    let config_dir = init_config_dir();
    core.config.audio_device = core.audio.device_name.to_owned();

    if let Ok(output_dir) = config_dir {
        // TODO: Fix me
        core.config.write(&output_dir, "global.cfg").unwrap();
    }

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
pub unsafe extern "C" fn hippo_play_file(_core: *mut HippoCore, _filename: *const c_char) {
    //let core = &mut *_core;
    //let slice = CStr::from_ptr(filename);

    println!("trying to play unspported callback");

    //core.play_file(slice.to_str().unwrap(), 0);
    //println!("filename to play returned: {}", slice.to_str().unwrap());
}

#[no_mangle]
pub unsafe extern "C" fn hippo_init_audio_device(_core: *mut HippoCore) -> *const c_char {
    let core = &mut *_core;
    if let Err(e) = core.init_audio_device() {
        let text = format!("{:#?}", e);
        error!("Error setting up audio device {}", text);
        if let Ok(c_string) = CString::new(text) {
            core.error_string = Some(c_string);
            return core
                .error_string
                .as_ref()
                .map_or_else(|| std::ptr::null(), |v| v.as_ptr());
        }
    }

    std::ptr::null()
}

#[no_mangle]
pub unsafe extern "C" fn hippo_service_api_new(
    _core: *mut HippoCore,
) -> *const ffi::HippoServiceAPI {
    let core = &mut *_core;
    PluginService::new_c_api(core.song_db)
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
pub unsafe extern "C" fn hippo_playlist_remove_entries(
    core: *mut HippoCore,
    entry: i32,
    _count: i32,
) {
    let core = &mut *core;
    core.playlist.remove_entry(entry);
}

#[no_mangle]
pub unsafe extern "C" fn hippo_playlist_count(core: *mut HippoCore) -> u32 {
    let core = &mut *core;
    core.playlist.entries.len() as u32
}

#[no_mangle]
pub unsafe extern "C" fn hippo_playlist_get(
    core: *mut HippoCore,
    row: i32,
    col: i32,
    len: *mut i32,
) -> *const u8 {
    let core = &mut *core;

    if row >= core.playlist.entries.len() as i32 || row < 0 {
        std::ptr::null()
    } else {
        match col {
            0 => {
                let entry = &core.playlist.entries[row as usize].title;
                *len = entry.len() as i32;
                entry.as_ptr()
            },

            1 => {
                let entry = &core.playlist.entries[row as usize].duration_string;
                *len = entry.len() as i32;
                entry.as_ptr()
            },

            2 => {
                let entry = &core.playlist.entries[row as usize].song_type;
                *len = entry.len() as i32;
                entry.as_ptr()
            },

            _ => std::ptr::null(),
        }
    }
}
