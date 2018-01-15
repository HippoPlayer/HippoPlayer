extern crate rodio;

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

use plugin_handler::{Plugins};
use audio::{HippoAudio, MusicInfo};
use std::path::Path;
use std::env;

use wrui::{SharedLibUi, Ui};
use wrui::wrui::*;
use playerview::PlayerView;
use playlist::PlaylistView;

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    main_widget: Widget,
    player_view: PlayerView,
    playlist: PlaylistView,
    ui: Ui,
    app: Application,
    current_song_time: f32,
}

#[cfg(target_os="windows")]
fn get_wrui_path() -> &'static str {
    "t2-output/win64-msvc-debug-default/wrui_qt.dll"
}

#[cfg(target_os="macos")]
fn get_wrui_path() -> &'static str {
    "t2-output/macosx-clang-debug-default/libwrui_qt.dylib"
}

impl <'a> HippoPlayer<'a> {
    pub fn new(ui: Ui) -> HippoPlayer<'a> {
        HippoPlayer {
            audio: HippoAudio::new(),
            plugins: Plugins::new(),
            app: ui.create_application(),
            main_widget: ui.create_widget(),
            player_view: PlayerView::new(ui),
            playlist: PlaylistView::new(ui),
            ui: ui,
            current_song_time: 0.0,
        }
    }

    fn select_song(&mut self, item: &ListWidgetItem) {
        let info = self.play_file(&item.get_string_data());
        self.current_song_time = info.duration as f32;

        self.player_view.set_current_time(self.current_song_time);
        self.player_view.set_title(&info.title);

        self.playlist.select_song(item, &info);
    }

    fn per_sec_update(&mut self) {
        self.player_view.set_current_time(self.current_song_time);
        self.current_song_time -= 1.0;
    }

    fn before_quit(&mut self) {
        self.playlist.save_playlist("playlist.json");
    }

    pub fn run(&mut self) {
        let main_window = self.ui.create_main_window();

        self.player_view.setup();
        self.playlist.setup();

        self.playlist.load_playlist("playlist.json");

        let timer = self.ui.create_timer();
        let layout = self.ui.create_v_box_layout();

        layout.add_widget(&self.player_view.widget);
        layout.add_widget(&self.playlist.widget);

        set_timeout_event!(timer, self, HippoPlayer, HippoPlayer::per_sec_update);
        set_item_double_clicked_event!(self.playlist.widget, self, HippoPlayer, HippoPlayer::select_song);
        set_about_to_quit_event!(self.app, self, HippoPlayer, HippoPlayer::before_quit);

        timer.start(1000);

        self.main_widget.set_layout(&layout);

        let player_window = self.ui.create_frameless_window();
        main_window.set_central_widget(&self.main_widget);

        player_window.set_content(&main_window);
        player_window.set_window_title("HippoPlayer 0.1");

        player_window.resize(500, 800);
        player_window.show();

        self.app.exec();
    }

    pub fn play_file(&mut self, filename: &str) -> MusicInfo {
        let path = Path::new(filename);
        // TODO: Proper error handling
        let file_ext = path.extension().unwrap().to_str().unwrap();

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.is_ext_supported(file_ext) {
                // This is a bit hacky right now but will do the trick
                self.audio.stop();
                self.audio = HippoAudio::new();
                let info = self.audio.start_with_file(&plugin, filename);
                return info;
            }
        }

        println!("Unable to find plugin to support {}", filename);

        MusicInfo::default()
    }
}



fn main() {
    let _args: Vec<String> = env::args().collect();

    let wrui_instance = SharedLibUi::new(get_wrui_path()).unwrap();
    let ui = wrui_instance.get_ui();

    let mut app = HippoPlayer::new(ui);

    app.plugins.add_decoder_plugin("OpenMPT");
    app.plugins.add_decoder_plugin("HivelyPlugin");
    app.plugins.add_decoder_plugin("SidPlugin");

    /*
    if args.len() > 1 {
        app.play_file(&args[1]);
    } else {
        //app.play_file("bin/player/songs/mod/global_trash_3_v2.mod");
        app.play_file("bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx");
    }
    */

    app.run();
}

