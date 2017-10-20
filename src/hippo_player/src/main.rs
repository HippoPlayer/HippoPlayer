extern crate minifb;
extern crate rodio;
extern crate dynamic_reload;

mod plugin_handler;
mod audio;

use plugin_handler::{Plugins};
use audio::HippoAudio;
use minifb::{Key, WindowOptions, Window};

//use std::sync::mpsc::{channel, Receiver, Sender};
//use marker::{Send, Sync};
//use std::fs::OpenOptions;

const WIDTH: usize = 640;
const HEIGHT: usize = 360;

struct HippoPlayer<'a> {
    window: minifb::Window,
    audio: HippoAudio,
    plugins: Plugins<'a>,
    buffer: Vec<u32>,
}

impl <'a> HippoPlayer<'a> {
    pub fn new() -> HippoPlayer<'a> {
        let buffer: Vec<u32> = vec![0; WIDTH * HEIGHT];

        let window = Window::new("HippoPlayer - ESC to exit",
                                 WIDTH,
                                 HEIGHT,
                                 WindowOptions::default()).unwrap_or_else(|e| {
            panic!("{}", e);
        });

        HippoPlayer {
            window: window,
            audio: HippoAudio::new(),
            plugins: Plugins::new(),
            buffer,
        }
    }

    pub fn update(&mut self) {
        while self.window.is_open() && !self.window.is_key_down(Key::Escape) {
            for i in self.buffer.iter_mut() {
                *i = 0; // write something more funny here!
            }

            // We unwrap here as we want this code to exit if it fails. Real applications may want to handle this in a different way
            self.window.update_with_buffer(&self.buffer).unwrap();
        }
    }
}

fn main() {
    let mut app = HippoPlayer::new();

    app.plugins.add_decoder_plugin("OpenMPT");
    app.plugins.add_decoder_plugin("HivelyPlugin");

    //app.audio.start_with_file(&app.plugins.decoder_plugins[0], "bin/player/songs/mod/global_trash_3_v2.mod");
    app.audio.start_with_file(&app.plugins.decoder_plugins[1], "bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx");

    // Hacky set up for playing some music

    //let sink = Sink::new(&app.audio_endpoint);

    // Add a dummy source of the sake of the example.
    //let source = HippoPlayback::new(&app.plugins.decoder_plugins[0], app.audio_send.clone());
    //sink.append(source);

    //app.audio_sink = Some(sink);

    app.update();
}
