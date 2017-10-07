extern crate minifb;
extern crate rodio;

use rodio::{Source, Endpoint, Sink};
use minifb::{Key, WindowOptions, Window};

const WIDTH: usize = 640;
const HEIGHT: usize = 360;

use std::time::Duration;

/// An infinite source that produces a sine.
///
/// Always has a rate of 48kHz and one channel.
#[derive(Clone, Debug)]
pub struct HippoPlayback {
    freq: f32,
    num_sample: usize,
}

impl HippoPlayback {
    /// The frequency of the sine.
    #[inline]
    pub fn new(freq: u32) -> HippoPlayback {
        HippoPlayback {
            freq: freq as f32,
            num_sample: 0,
        }
    }
}

impl Iterator for HippoPlayback {
    type Item = f32;

    #[inline]
    fn next(&mut self) -> Option<f32> {
        self.num_sample = self.num_sample.wrapping_add(1);

        //println!("next sample!");

        let value = 2.0 * 3.14159265 * self.freq * self.num_sample as f32 / 48000.0;
        Some(value.sin())
    }
}

impl Source for HippoPlayback {
    #[inline]
    fn current_frame_len(&self) -> Option<usize> {
        None
    }

    #[inline]
    fn channels(&self) -> u16 {
        1
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

struct HippoPlayer {
    window: minifb::Window,
    audio_endpoint: rodio::Endpoint,
    audio_sink: rodio::Sink,
    buffer: Vec<u32>,
}

impl HippoPlayer {
    pub fn new() -> HippoPlayer {
        let buffer: Vec<u32> = vec![0; WIDTH * HEIGHT];

        let window = Window::new("HippoPlayer - ESC to exit",
                                    WIDTH,
                                    HEIGHT,
                                    WindowOptions::default()).unwrap_or_else(|e| {
            panic!("{}", e);
        });

        let endpoint = rodio::get_default_endpoint().unwrap();
        let sink = Sink::new(&endpoint);

        // Add a dummy source of the sake of the example.
        let source = HippoPlayback::new(440);
        sink.append(source);

        HippoPlayer {
            window: window,
            audio_endpoint: endpoint,
            audio_sink: sink,
            buffer,
        }
    }

    pub fn update(&mut self) {
        while self.window.is_open() && !self.window.is_key_down(Key::Escape) {
            for i in self.buffer.iter_mut() {
                *i = 0; // write something more funny here!
            }

            println!("window update!");

            // We unwrap here as we want this code to exit if it fails. Real applications may want to handle this in a different way
            self.window.update_with_buffer(&self.buffer).unwrap();
        }
    }
}

fn main() {
    let mut app = HippoPlayer::new();

    app.update();
}