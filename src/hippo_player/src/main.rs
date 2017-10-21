#[macro_use]
extern crate wrui;
extern crate rodio;
extern crate dynamic_reload;

mod plugin_handler;
mod audio;

use plugin_handler::{Plugins};
use audio::HippoAudio;
use wrui::{Wrui, Window, Pos, Color};
use std::path::Path;
use std::env;

//use std::sync::mpsc::{channel, Receiver, Sender};
//use marker::{Send, Sync};
//use std::fs::OpenOptions;

struct HippoPlayer<'a> {
    window: Window,
    audio: HippoAudio,
    plugins: Plugins<'a>,
    wrui: &'a Wrui,
}

impl <'a> HippoPlayer<'a> {
    pub fn new(wrui: &Wrui) -> HippoPlayer {
        HippoPlayer {
            window: wrui.create_window(),
            audio: HippoAudio::new(),
            plugins: Plugins::new(),
            wrui: wrui,
        }
    }

    pub fn setup(&mut self) {
        window_set_paint_event!(self.window, self, HippoPlayer, HippoPlayer::paint_event);
    }

    pub fn paint_event(&mut self) {
        let painter = self.wrui.get_painter();
        painter.draw_text(Pos::new(10.0, 10.0), Color::new(1.0, 1.0, 0.0, 1.0), "test text!");
    }

    pub fn run(&self) {
        self.wrui.run();
    }

    pub fn play_file(&mut self, filename: &str) {
        let path = Path::new(filename);
        // TODO: Proper error handling
        let file_ext = path.extension().unwrap().to_str().unwrap();

        // find a plugin that supports the file

        for plugin in &self.plugins.decoder_plugins {
            if plugin.is_ext_supported(file_ext) {
                self.audio.start_with_file(&plugin, filename); 
                return;
            }
        }

        println!("Unable to find plugin to support {}", filename);
    }
}



fn main() {
    // very temporary for now while testing
    let wrui = Wrui::new("../../wrui/t2-output/macosx-clang-debug-default/libwrui_dimgui.dylib").unwrap();

    let args: Vec<String> = env::args().collect();
    let mut app = HippoPlayer::new(&wrui);

    app.plugins.add_decoder_plugin("OpenMPT");
    app.plugins.add_decoder_plugin("HivelyPlugin");
    app.plugins.add_decoder_plugin("SidPlugin");

    if args.len() > 1 {
        app.play_file(&args[1]);
    } else {
        //app.play_file("bin/player/songs/mod/global_trash_3_v2.mod");
        app.play_file("bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx");
    }

    app.setup();
    app.run();
}

