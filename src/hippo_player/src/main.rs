extern crate minifb;
extern crate rodio;
extern crate dynamic_reload;

mod plugin_handler;

use plugin_handler::{Plugins, DecoderPlugin};
use std::os::raw::{c_void};

use rodio::{Source, Sink};
use minifb::{Key, WindowOptions, Window};

use std::time::Duration;
use std::sync::mpsc::{channel, Receiver, Sender};
//use marker::{Send, Sync};
//use std::fs::OpenOptions;

const WIDTH: usize = 640;
const HEIGHT: usize = 360;

#[derive(Clone)]
pub enum DecodeEvent {
    Position(usize),
    Data(Vec<u8>),
}

#[derive(Clone)]
pub struct HippoPlayback {
    plugin_user_data: u64,
    plugin: DecoderPlugin,
    temp_data: Vec<i16>,
    out_data: Vec<f32>,
    frame_size: usize,
    current_offset: usize,
    sender: Sender<DecodeEvent>,
}

impl HippoPlayback {
    /// The frequency of the sine.
    #[inline]
    pub fn new(plugin: &DecoderPlugin, sender: Sender<DecodeEvent>) -> HippoPlayback {
        let user_data = ((plugin.plugin_funcs).create)() as u64;
        let ptr_user_data = user_data as *mut c_void;
        let frame_size = (((plugin.plugin_funcs).frame_size)(ptr_user_data) * 2) as usize;
        //let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, b"bin/player/songs/ahx/geir_tjelta_-_a_new_beginning.ahx\0".as_ptr());
        let _open_state = ((plugin.plugin_funcs).open)(ptr_user_data, b"bin/player/songs/mod/global_trash_3_v2.mod\0".as_ptr());

        HippoPlayback {
            plugin_user_data: user_data,
            plugin: plugin.clone(),
            temp_data: vec![0; frame_size],
            out_data: vec![0.0; frame_size],
            frame_size: frame_size,
            current_offset: frame_size + 1,
            sender: sender,
        }
    }
}

impl Iterator for HippoPlayback {
    type Item = f32;

    #[inline]
    fn next(&mut self) -> Option<f32> {
        self.current_offset += 1;

        if self.current_offset >= self.frame_size {
            ((self.plugin.plugin_funcs).read_data)(self.plugin_user_data as *mut c_void, self.temp_data.as_slice().as_ptr() as *mut u8);

            for i in 0 .. self.frame_size {
                self.out_data[i] = (self.temp_data[i] as f32) * 1.0 / 32768.0;
            }

            self.current_offset = 0;
        }

        Some(self.out_data[self.current_offset])

        /*
        self.num_sample = self.num_sample.wrapping_add(1);

        self.sender.send(DecodeEvent::Position(self.num_sample)).unwrap();

        //println!("next sample!");

        let value = 2.0 * 3.14159265 * self.freq * self.num_sample as f32 / 44100.0;
        Some(value.sin())
        */
    }
}

impl Source for HippoPlayback {
    #[inline]
    fn current_frame_len(&self) -> Option<usize> {
        None
    }

    #[inline]
    fn channels(&self) -> u16 {
        2
    }

    #[inline]
    fn samples_rate(&self) -> u32 {
        48000
    }

    #[inline]
    fn total_duration(&self) -> Option<Duration> {
        None
    }
}

struct HippoPlayer<'a> {
    window: minifb::Window,
    audio_endpoint: rodio::Endpoint,
    audio_sink: Option<rodio::Sink>,
    audio_send: Sender<DecodeEvent>,
    audio_recv: Receiver<DecodeEvent>,
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

        let (tx, rx) = channel();

        let endpoint = rodio::get_default_endpoint().unwrap();

        HippoPlayer {
            window: window,
            audio_endpoint: endpoint,
            audio_sink: None,
            audio_send: tx.clone(),
            audio_recv: rx,
            plugins: Plugins::new(),
            buffer,
        }
    }

    pub fn update(&mut self) {
        while self.window.is_open() && !self.window.is_key_down(Key::Escape) {
            for i in self.buffer.iter_mut() {
                *i = 0; // write something more funny here!
            }

            match self.audio_recv.try_recv() {
                Ok(event) => {
                    match event {
                        DecodeEvent::Position(pos) => println!("pos {}", pos),
                        _ => (),
                    }
                }

                _ => (),
            }

            // We unwrap here as we want this code to exit if it fails. Real applications may want to handle this in a different way
            self.window.update_with_buffer(&self.buffer).unwrap();
        }
    }
}


// HACK
//unsafe impl Send for std::os::raw::c_void {}
//unsafe impl Sync for std::os::raw::c_void {}

fn main() {
    let mut app = HippoPlayer::new();

    //app.plugins.add_decoder_plugin("HivelyPlugin");
    app.plugins.add_decoder_plugin("OpenMPT");

    // Hacky set up for playing some music

    let sink = Sink::new(&app.audio_endpoint);

    // Add a dummy source of the sake of the example.
    let source = HippoPlayback::new(&app.plugins.decoder_plugins[0], app.audio_send.clone());
    sink.append(source);

    app.audio_sink = Some(sink);

    app.update();
}
