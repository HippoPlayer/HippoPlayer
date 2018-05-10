#[macro_use]
extern crate hippo_api;

//#[macro_use]
extern crate rute;

use rute::rute::{PushButton};
use rute::PluginUi;

// TODO: Fix me
//use std::os::raw::c_void;

use hippo_api::view::View;
use hippo_api::service::Service;

struct Playlist {
    widget: Option<PushButton>,
    ui: Option<PluginUi>,
}

impl View for Playlist {
    fn new(_service: &Service) -> Playlist {
        Playlist {
            widget: None,
            ui: None,
        }
    }

    fn setup_ui(&mut self, ui: PluginUi) {
        let widget = ui.create_push_button();
        widget.set_text("player");

        /*
        set_push_button_pressed_event!(self.player_view.prev_button, self, HippoPlayer, HippoPlayer::prev_song);
        set_push_button_pressed_event!(self.player_view.play_button, self, HippoPlayer, HippoPlayer::play_song);
        set_push_button_pressed_event!(self.player_view.stop_button, self, HippoPlayer, HippoPlayer::stop_song);
        set_push_button_pressed_event!(self.player_view.next_button, self, HippoPlayer, HippoPlayer::next_song);
        */

        ui.get_parent().resize(500, 500);

        self.ui = Some(ui);
        self.widget = Some(widget)
    }

    fn destroy(&mut self) {
    }
}
/*
    fn select_song(&mut self, item: &ListWidgetItem) {
        /*
           let info = self.play_file(&item.get_string_data());
           self.current_song_time = info.duration as f32;
           self.is_playing = true;

           self.player_view.set_current_time(self.current_song_time);
           self.player_view.set_title(&info.title);

           self.playlist.select_song(item, &info);
           */
    }

    fn prev_song(&mut self) {
        /*
           let next_song = self.playlist.get_prev_song();

           if let Some(next) = next_song {
           self.select_song(&next);
           } else {
           self.is_playing = false;
           }
           */
    }

    fn play_song(&mut self) {
        /*
           if let Some(current) = self.playlist.widget.current_item() {
           self.select_song(&current);
           }
           */
    }

    fn next_song(&mut self) {
        /*
           let next_song = self.playlist.get_next_song();

           if let Some(next) = next_song {
           self.select_song(&next);
           } else {
           self.is_playing = false;
           }
           */
    }
*/

#[no_mangle]
pub fn hippo_view_plugin() -> *const std::os::raw::c_void {
    define_view_plugin!(PLUGIN, b"Player\0", b"0.0.1\0", Playlist);
    // Would be nice to get rid of this
    let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&PLUGIN) };
    ret
}

