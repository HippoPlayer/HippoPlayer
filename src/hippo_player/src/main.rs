extern crate rodio;
extern crate dynamic_reload;

mod plugin_handler;
mod audio;

use plugin_handler::{Plugins};
use audio::HippoAudio;
use std::path::Path;
use std::env;
use std::time::Duration;
use std::thread;

//use std::sync::mpsc::{channel, Receiver, Sender};
//use marker::{Send, Sync};
//use std::fs::OpenOptions;

struct HippoPlayer<'a> {
    audio: HippoAudio,
    plugins: Plugins<'a>,
}

impl <'a> HippoPlayer<'a> {
    pub fn new() -> HippoPlayer<'a> {
        HippoPlayer {
            audio: HippoAudio::new(),
            plugins: Plugins::new(),
        }
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
    let args: Vec<String> = env::args().collect();
    let mut app = HippoPlayer::new();

    app.plugins.add_decoder_plugin("OpenMPT");
    app.plugins.add_decoder_plugin("HivelyPlugin");
    app.plugins.add_decoder_plugin("SidPlugin");

    if args.len() > 1 {
        app.play_file(&args[1]);
    } else {
        //app.play_file("bin/player/songs/mod/global_trash_3_v2.mod");
        app.play_file("bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx");
    }

    println!("Playing.. waiting for ctrl-c");

    loop {
        thread::sleep(Duration::from_millis(10));
    }
}

