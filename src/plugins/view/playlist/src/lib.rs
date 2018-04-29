#[macro_use]
extern crate hippo_api;

use hippo_api::view::View;
use hippo_api::wrui::PluginUi;
use hippo_api::service::Service;

struct Playlist {
    _dummy: u32,
}

impl View for Playlist {
    fn new(_service: &Service) -> Playlist {
        Playlist { _dummy: 0 }
    }

    fn setup_ui(&mut self, ui: PluginUi) {
        let button = ui.create_push_button();
        button.set_text("foo from rust");
    }

    fn destroy(&mut self) {
        println!("Destroy plugin");
    }
}

#[no_mangle]
pub fn hippo_view_plugin() -> *const std::os::raw::c_void {
    define_view_plugin!(PLUGIN, b"Playlist\0", b"0.0.1\0", Playlist);
    // Would be nice to get rid of this
    let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&PLUGIN) };
    ret
}
