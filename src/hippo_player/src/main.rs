extern crate rodio;
extern crate dynamic_reload;

use std::os::raw::c_void;

#[macro_use]
extern crate wrui;

mod plugin_handler;
mod audio;
mod playerview;

use plugin_handler::{Plugins};
use audio::{HippoAudio, MusicInfo};
use std::path::Path;
use std::env;

use wrui::{SharedLibUi, Ui};
use wrui::wrui::*;
use playerview::PlayerView;

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
    playlist: ListWidget,
    main_widget: Widget,
    player_view: PlayerView,
    ui: Ui,
    app: Application,
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
            playlist: ui.create_list_widget(),
            main_widget: ui.create_widget(),
            player_view: PlayerView::new(ui),
            ui: ui,
        }
    }

    fn select_song(&mut self, item: &ListWidgetItem) {
        let info = self.play_file(&item.text());
        self.player_view.set_title(&info.title);
    }

    fn drag_enter(&mut self, event: &DragEnterEvent) {
        println!("Dropping files!");
        event.accept();
    }

    fn drop_files(&mut self, event: &DropEvent) {
        for url in event.mime_data().urls() {
            if url.is_local_file() {
                self.playlist.add_item(&url.to_local_file());
                //println!("Has local file {}", url.to_local_file());
            } else {
                println!("File is not local");
            }
        }

        event.accept_proposed_action();
    }

    pub fn run(&mut self) {
        let main_window = self.ui.create_main_window();

        self.player_view.setup();

        //main_window.resize(500, 500);
        //main_window.show();

        // Enable drag'n'drop on playlist

        self.playlist.set_drag_enabled(true);
        self.playlist.set_accept_drops(true);
        self.playlist.set_drop_indicator_shown(true);

        let layout = self.ui.create_v_box_layout();

        layout.add_widget(&self.player_view.widget);
        layout.add_widget(&self.playlist);

        set_drag_enter_event!(self.playlist, self, HippoPlayer, HippoPlayer::drag_enter);
        set_drop_event!(self.playlist, self, HippoPlayer, HippoPlayer::drop_files);
        set_item_double_clicked_event!(self.playlist, self, HippoPlayer, HippoPlayer::select_song);

        //let window = self.ui.create_widget();
        self.main_widget.set_layout(&layout);
        main_window.set_central_widget(&self.main_widget);

        let player_window = self.ui.create_frameless_window();
        player_window.set_content(&main_window);
        player_window.set_window_title("HippoPlayer 0.1");
        player_window.show();

        player_window.resize(500, 800);

        //main_window.show();

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

