extern crate pancurses;
extern crate argparse;
extern crate hippo_core;
extern crate hippo_api;

use hippo_core::DecoderPlugins;
use std::env;
use std::path::Path;
use argparse::{ArgumentParser, Store};

#[derive(Default)]
struct Options {
    song: String,
    playlist: String,
}

struct HippoTerminal<'a> {
    decoder_plugins: DecoderPlugins<'a>,
}

fn main() {
    let current_path = env::current_dir().unwrap();

    let current_exe = std::env::current_exe().unwrap();
    std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();

    let mut options = Options::default();
    {
        let mut ap = ArgumentParser::new();
        ap.set_description("HippoPlayer terminal interface");
        ap.refer(&mut options.song)
            .add_option(&["-s", "--song"], Store,
            "Play a song");
        ap.refer(&mut options.playlist)
            .add_option(&["-l", "--list"], Store,
            "Play a playlist");
        ap.parse_args_or_exit();
    }

    let hippo_terminal = HippoTerminal {
        decoder_plugins: DecoderPlugins::new(),
    };

    if !Path::new("bin").is_dir() {
        env::set_current_dir(current_path).unwrap();
    }
}
