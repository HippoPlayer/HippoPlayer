extern crate pancurses;
extern crate argparse;
extern crate hippo_core;
extern crate hippo_api;

use hippo_core::HippoCore;
use std::env;
use std::path::Path;
use argparse::{ArgumentParser, Store};

#[derive(Default)]
struct Options {
    song: String,
    playlist: String,
}

struct HippoTerminal<'a> {
    core: HippoCore<'a>
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

    let mut hippo_terminal = HippoTerminal {
        core: HippoCore::new(),
    };

    if !Path::new("bin").is_dir() {
        env::set_current_dir(current_path).unwrap();
    }

    if !options.song.is_empty() {
        hippo_terminal.core.play_file(&options.song);

        println!("Playing {}", options.song);

        loop {

        }
    }
}
