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
        widget.set_text("Song Info");

        ui.get_parent().resize(500, 500);

        self.ui = Some(ui);
        self.widget = Some(widget)
    }

    fn destroy(&mut self) {
    }
}

#[no_mangle]
pub fn hippo_view_plugin() -> *const std::os::raw::c_void {
    define_view_plugin!(PLUGIN, b"Song Info\0", b"0.0.1\0", Playlist);
    // Would be nice to get rid of this
    let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&PLUGIN) };
    ret
}


