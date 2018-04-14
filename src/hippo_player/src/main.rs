extern crate rodio;
extern crate walkdir;

#[macro_use]
extern crate serde_derive;
extern crate serde;
extern crate serde_json;

#[macro_use]
extern crate wrui;

extern crate dynamic_reload;

use std::os::raw::c_void;

mod plugin_handler;
mod audio;
mod playerview;
mod playlist;
mod song_db;
mod song_info;
pub mod service;

use plugin_handler::{Plugins};
use audio::{HippoAudio, MusicInfo};
use std::env;
use std::fs;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;

use wrui::{SharedLibUi, Ui};
use wrui::wrui::*;
use playerview::PlayerView;
use playlist::PlaylistView;
use song_info::SongInfoView;

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    plugin_service: service::PluginService,
    main_widget: Widget,
    player_view: PlayerView,
    playlist: PlaylistView,
    song_info_view: SongInfoView,
    tool_window_manager: ToolWindowManager,
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
            plugin_service: service::PluginService::new(),
            app: ui.create_application(),
            main_widget: ui.create_widget(),
            player_view: PlayerView::new(ui),
            playlist: PlaylistView::new(ui),
            song_info_view: SongInfoView::new(ui),
            tool_window_manager: ui.create_tool_window_manager(),
            ui: ui,
            current_song_time: -10.0,
            is_playing: false,
        }
    }

    fn select_song(&mut self, item: &ListWidgetItem) {
        let info = self.play_file(&item.get_string_data());
        self.current_song_time = info.duration as f32;
        self.is_playing = true;

        self.player_view.set_current_time(self.current_song_time);
        self.player_view.set_title(&info.title);

        self.playlist.select_song(item, &info);
    }

    fn per_sec_update(&mut self) {
        self.player_view.set_current_time(self.current_song_time);

        if self.is_playing {
            self.current_song_time -= 1.0;
        }

        if self.current_song_time < 0.0 && self.current_song_time > -9.0 {
            self.next_song();
        }
    }

    fn stop_song(&mut self) {
        self.audio.stop();
        self.is_playing = false;
    }

    fn prev_song(&mut self) {
        let next_song = self.playlist.get_prev_song();

        if let Some(next) = next_song {
            self.select_song(&next);
        } else {
            self.is_playing = false;
        }
    }

    fn play_song(&mut self) {
        if let Some(current) = self.playlist.widget.current_item() {
            self.select_song(&current);
        }
    }

    fn next_song(&mut self) {
        let next_song = self.playlist.get_next_song();

        if let Some(next) = next_song {
            self.select_song(&next);
        } else {
            self.is_playing = false;
        }
    }

    fn before_quit(&mut self) {
        if let Err(err) = self.playlist.save_playlist("playlist.json") {
            println!("Unable to save playlist {:?}", err);
        }
    }

    fn add_files(&mut self) {
        for url in self.app.get_files().iter().filter(|u| u.is_local_file()) {
            self.playlist.add_file(&url.to_local_file());
        }
    }

    fn show_plugin(&mut self, action: &Action) {
        let widget = self.ui.create_widget();
        let plugin = &self.plugins.view_plugins[action.get_int_data() as usize];

        let _instance = plugin.create_instance(self.ui.get_c_api(), &self.plugin_service, widget.obj.unwrap());

        println!("Showing plugin {}", action.get_int_data());
    }

    fn create_plugins_menu(&mut self) -> Menu {
        let plugin_menu = self.ui.create_menu();
        plugin_menu.set_title("Views");

        for (_i, plugin) in self.plugins.view_plugins.iter().enumerate() {
            let name = plugin.get_name();

            let action = self.ui.create_action();
            action.set_text(&name);

            set_menu_triggered_event!(plugin_menu, self, HippoPlayer, HippoPlayer::show_plugin);

            plugin_menu.add_action(&action);
        }

        plugin_menu
    }

    pub fn run(&mut self) {
        self.app.set_style_sheet("bin/player/themes/dark/style.qss");

        let main_window = self.ui.create_main_window();

        self.player_view.setup();
        self.playlist.setup();

        if let Err(err) = self.playlist.load_playlist("playlist.json") {
            println!("Unable to load playlist {:?}", err);
        }

        let timer = self.ui.create_timer();
        let layout = self.ui.create_v_box_layout();

        //layout.add_widget(&self.player_view.widget);
        //layout.add_widget(&self.playlist.widget);

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

        set_push_button_pressed_event!(self.player_view.prev_button, self, HippoPlayer, HippoPlayer::prev_song);
        set_push_button_pressed_event!(self.player_view.play_button, self, HippoPlayer, HippoPlayer::play_song);
        set_push_button_pressed_event!(self.player_view.stop_button, self, HippoPlayer, HippoPlayer::stop_song);
        set_push_button_pressed_event!(self.player_view.next_button, self, HippoPlayer, HippoPlayer::next_song);

        set_timer_timeout_event!(timer, self, HippoPlayer, HippoPlayer::per_sec_update);
        set_action_triggered_event!(add_files, self, HippoPlayer, HippoPlayer::add_files);

        set_list_widget_item_double_clicked_event!(self.playlist.widget, self, HippoPlayer, HippoPlayer::select_song);
        set_application_about_to_quit_event!(self.app, self, HippoPlayer, HippoPlayer::before_quit);

        timer.start(1000);

        self.main_widget.set_layout(&layout);
        self.main_widget.resize(1200, 1000);

        //let player_window = self.ui.create_widget();
        main_window.set_central_widget(&self.main_widget);

        self.tool_window_manager.set_parent(&self.main_widget);

        self.tool_window_manager.add_to_docking(&self.player_view.widget);
        self.tool_window_manager.add_to_docking(&self.playlist.widget);
        self.tool_window_manager.add_to_docking(&self.song_info_view.view);

        layout.add_widget(&self.tool_window_manager);

        //player_window.set_content(&main_window);
        //player_window.set_window_title(&format!("HippoPlayer 0.0.1 {}", build_id()));

        main_window.resize(1200, 1000);
        main_window.show();

        // temp create a instance for testing

        /*
        for plugin in &self.plugins.view_plugins {
            let _user_data = ((plugin.plugin_funcs).create)(self.plugin_service.get_c_service_api(), self.ui.get_c_api()) as u64;
        }
        */

        //main_window.show();
        //self.song_info_view.show();

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

                self.song_info_view.update_data(filename, &self.plugin_service.get_song_db());
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

    let wrui_instance = SharedLibUi::new().unwrap();
    let ui = wrui_instance.get_ui();

    let mut app = HippoPlayer::new(ui);

    unsafe {
        let service_api = app.plugin_service.get_c_service_api();
        println!("{:?}", service_api);
        ((*service_api).get_io_api)((*service_api).private_data, 1);
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

