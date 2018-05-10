extern crate rodio;
extern crate walkdir;
extern crate hippo_api;

#[macro_use]
extern crate serde_derive;
extern crate serde;
extern crate serde_json;
extern crate rmp_rpc;
pub extern crate rmp;

#[macro_use]
extern crate rute;

extern crate dynamic_reload;

use std::os::raw::c_void;

mod audio;
mod playerview;
mod playlist;
mod playlist_view;
mod plugin_handler;
mod song_db;
mod song_info;

mod service;
pub mod service_ffi;

pub use rmp as msgpack;

use plugin_handler::{Plugins};
use audio::{HippoAudio, MusicInfo};
use std::env;
use std::fs;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use playlist::Playlist;
use std::io;

use rute::{SharedLibUi, Ui};
use rute::rute::*;
//use playerview::PlayerView;
//use playlist_view::PlaylistView;
//use song_info::SongInfoView;

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    plugin_service: service_ffi::PluginService,
    main_widget: Widget,
    playlist: Playlist,
    dock_manager: DockManager,
    ui: Ui,
    app: Application,
    current_song_time: f32,
    is_playing: bool,
}

include!(concat!(env!("OUT_DIR"), "/build_id.rs"));

impl <'a> HippoPlayer<'a> {
    pub fn new(ui: Ui) -> HippoPlayer<'a> {
        HippoPlayer {
           audio: HippoAudio::new(),
           plugins: Plugins::new(),
           plugin_service: service_ffi::PluginService::new(),
           app: ui.create_application(),
           main_widget: ui.create_widget(),
           playlist: Playlist::new(),
           dock_manager: ui.create_dock_manager(),
           ui: ui,
           current_song_time: -10.0,
           is_playing: false,
        }
    }

    fn per_sec_update(&mut self) {
        //self.player_view.set_current_time(self.current_song_time);

        /*
        if self.is_playing {
            self.current_song_time -= 1.0;
        }
        */

        /*
        if self.current_song_time < 0.0 && self.current_song_time > -9.0 {
            self.next_song();
        }
        */
    }

    fn stop_song(&mut self) {
        self.audio.stop();
        self.is_playing = false;
    }

    fn try_save_layout(filename: &str, layout: &str) -> io::Result<()> {
        let mut file = File::create(filename)?;
        file.write_all(layout.as_bytes())?;
        Ok(())
    }

    fn before_quit(&mut self) {
        let state = self.dock_manager.save_state();

        println!("state {}", state);

        // TODO: Error handling
        Self::try_save_layout("layout.data", &state).unwrap();

        //let state = self.tool_window_manager.save_state();
        //println!("tool_window_mangare state {}", state);
        /*
           if let Err(err) = self.playlist.save_playlist("playlist.json") {
           println!("Unable to save playlist {:?}", err);
           }
           */
    }

    fn add_files(&mut self) {
        /*
           for url in self.app.get_files().iter().filter(|u| u.is_local_file()) {
           self.playlist.add_file(&url.to_local_file());
           }
           */
    }

    fn show_plugin(&mut self, action: &Action) {
        let plugin_index = action.get_int_data() as usize;
        let widget = self.ui.create_widget();
        let plugin = &self.plugins.view_plugins[plugin_index];

        let _instance = plugin.create_instance(&self.ui, &self.plugin_service, &widget);

        let dock_widget = self.ui.create_dock_widget();

        dock_widget.set_object_name(&action.text());
        dock_widget.set_widget(&widget);
        self.dock_manager.add_to_docking(&dock_widget);

        widget.resize(500, 500);
        widget.show();

        widget.set_persist_data("pls save me pls!");

        println!("Showing plugin {}", plugin_index);
    }

    fn create_plugins_menu(&mut self) -> Menu {
        let plugin_menu = self.ui.create_menu();
        plugin_menu.set_title("Views");

        set_menu_triggered_event!(plugin_menu, self, HippoPlayer, HippoPlayer::show_plugin);

        for (i, plugin) in self.plugins.view_plugins.iter().enumerate() {
            let name = plugin.get_name();

            let action = self.ui.create_action();
            action.set_text(&name);
            action.set_int_data(i as i32);

            plugin_menu.add_action(&action);
        }

        plugin_menu
    }

    ///
    /// This will load a defult layout file and set up plugins for it
    ///
    fn load_layout(&mut self, filename: &str) {
        let mut layout_state = String::new();
        let mut file = match File::open(filename) {
            Ok(file) => file,
            Err(_) => {
                println!("Unable to open layout {}", filename);
                return;
            }
        };

        file.read_to_string(&mut layout_state).unwrap();

        self.dock_manager.restore_state(&layout_state);

        for dock_widget in self.dock_manager.get_dock_widgets() {
            let name = dock_widget.object_name();

            for plugin in &self.plugins.view_plugins {
                if plugin.get_name() == name {
                    continue;
                }

                let widget = self.ui.create_widget();
                let _instance = plugin.create_instance(&self.ui, &self.plugin_service, &widget);

                dock_widget.set_widget(&widget);

                widget.set_persist_data("pls save me pls!");
                widget.resize(500, 500);
            }
        }
    }

    pub fn run(&mut self) {
        self.app.set_style_sheet("bin/player/themes/dark/style.qss");

        let main_window = self.ui.create_main_window();
        let timer = self.ui.create_timer();

         let layout = self.ui.create_v_box_layout();

        let add_files = self.ui.create_action();
        add_files.set_shortcut_mod(Keys::KeyO, MetaKeys::Ctrl);
        add_files.set_text("Add Files");

        let file_menu = self.ui.create_menu();
        file_menu.set_title("File");
        file_menu.add_action(&add_files);

        let plugin_menu = self.create_plugins_menu();

        let menu_bar = main_window.menu_bar();
        menu_bar.add_menu(&file_menu);
        menu_bar.add_menu(&plugin_menu);

        set_timer_timeout_event!(timer, self, HippoPlayer, HippoPlayer::per_sec_update);
        set_action_triggered_event!(add_files, self, HippoPlayer, HippoPlayer::add_files);
        set_application_about_to_quit_event!(self.app, self, HippoPlayer, HippoPlayer::before_quit);

        timer.start(1000);

        self.main_widget.set_layout(&layout);
        self.main_widget.resize(1200, 1000);

        main_window.set_central_widget(&self.main_widget);

        self.dock_manager.set_parent(&self.main_widget);

        layout.add_widget(&self.dock_manager);

        //player_window.set_content(&main_window);
        //player_window.set_window_title(&format!("HippoPlayer 0.0.1 {}", build_id()));

        main_window.resize(1200, 1000);

        self.load_layout("layout.data");


        main_window.show();

        self.app.exec();
    }

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
                let info = self.audio.start_with_file(&plugin, &self.plugin_service, filename);

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
                let info = self.audio.start_with_file(&plugin, &self.plugin_service, filename);
                return info;
            }
        }

        println!("Unable to find plugin to support {}", filename);

        MusicInfo::default()
    }
}

impl<'a> Drop for HippoPlayer<'a> {
    fn drop(&mut self) {
        self.audio.stop();
    }
}

fn main() {
    let _args: Vec<String> = env::args().collect();

    let current_path = std::env::current_dir().unwrap();

    let rute_instance = SharedLibUi::new().unwrap();
    let ui = rute_instance.get_ui();

    let mut app = HippoPlayer::new(ui);

    unsafe {
        let service_api = app.plugin_service.get_c_service_api();
        println!("{:?}", service_api);
        ((*service_api).get_io_api)((*service_api).priv_data, 1);
    }

    app.plugins.add_plugins_from_path();

    // No need to switch back if we are in the correct spot

    if !Path::new("bin").is_dir() {
        std::env::set_current_dir(current_path).unwrap();
    }

    app.run();

    // we exit directly here because of Qt and the way its static destructors works
    // causes lots of issues because we have the Qt code in a separate dll
    std::process::exit(0);
}

