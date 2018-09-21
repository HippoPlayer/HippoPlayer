extern crate pancurses;
extern crate argparse;

#[derive(Default)]
struct Options {
    song: String,
    playlist: String,
}

use argparse::{ArgumentParser, Store};

fn main() {
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
}
