#[macro_use]
extern crate hippo_api;

#[macro_use]
extern crate rute;

use std::path::Path;
use std::ffi::OsStr;
use rute::rute::{ListWidget, DragEnterEvent, DropEvent};
use rute::PluginUi;

// TODO: Fix me
use std::os::raw::c_void;

use hippo_api::view::View;
use hippo_api::service::Service;

struct Playlist {
    widget: Option<ListWidget>,
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
        let widget = ui.create_list_widget();

        widget.set_drag_enabled(true);
        widget.set_accept_drops(true);
        widget.set_drop_indicator_shown(true);

        set_drag_enter_event!(widget, self, Playlist, Playlist::drag_enter);
        set_drop_event!(widget, self, Playlist, Playlist::drop_files);

        ui.get_parent().resize(500, 500);

        self.ui = Some(ui);
        self.widget = Some(widget)
    }

    fn destroy(&mut self) {
        println!("Destroy plugin");
    }
}

impl Playlist {
    fn get_filename_only(filename: &str) -> Option<&str> {
        Path::new(filename).file_name().and_then(OsStr::to_str)
    }

    fn drop_files(&mut self, event: &DropEvent) {
        for url in event.mime_data().urls().iter().filter(|u| u.is_local_file()) {
            self.add_file(&url.to_local_file());
        }

        event.accept_proposed_action();
    }

    fn add_file(&mut self, local_file: &str) {
        let ui = self.ui.unwrap();

        if let Some(filename) = Self::get_filename_only(&local_file) {
            let item = ui.create_list_widget_item();

            // Check if we already have this file added then we can use the title again
            // TODO: Request song name for this file
            item.set_text(filename);
            item.set_string_data(&local_file);

            self.widget.as_ref().unwrap().add_item(&item);
        } else {
            println!("Skipped adding {} as no proper filename was found", local_file);
        }
    }

    fn drag_enter(&mut self, event: &DragEnterEvent) {
        event.accept();
    }
}

#[no_mangle]
pub fn hippo_view_plugin() -> *const std::os::raw::c_void {
    define_view_plugin!(PLUGIN, b"Playlist\0", b"0.0.1\0", Playlist);
    // Would be nice to get rid of this
    let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&PLUGIN) };
    ret
}
