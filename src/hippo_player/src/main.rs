extern crate minifb;
extern crate rodio;

use rodio::{Source, Endpoint, Sink};
use minifb::{Key, WindowOptions, Window};

const WIDTH: usize = 640;
const HEIGHT: usize = 360;

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
        let source = rodio::source::SineWave::new(440);
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

            // We unwrap here as we want this code to exit if it fails. Real applications may want to handle this in a different way
            self.window.update_with_buffer(&self.buffer).unwrap();
        }
    }
}

fn main() {
    let mut app = HippoPlayer::new();

    app.update();
}
