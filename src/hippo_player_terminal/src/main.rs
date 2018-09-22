extern crate pancurses;
extern crate argparse;
extern crate hippo_core;
//extern crate hippo_api;

use argparse::{ArgumentParser, Store};
use hippo_core::HippoCore;
use pancurses::{initscr, endwin, Input, curs_set};
use std::env;
use std::path::Path;

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
        let window = initscr();

        //let (width, height) = window.get_max_yx();

        //window.printw("Hello Rust");

        hippo_terminal.core.play_file(&options.song);

        //let y_pos = 0;

        loop {
            window.clear();
            window.draw_box('|', '-');
            window.mvaddstr(2, 2, format!("Playing.....: {}", &options.song));

            window.refresh();
            match window.getch() {
                Some(Input::Character('\x1B')) => {
                    break;
                }

                _ => (),
            }

            // TODO: Calculate sleep time
            pancurses::napms(1);
        }

        curs_set(1);
        endwin();
    }
}
