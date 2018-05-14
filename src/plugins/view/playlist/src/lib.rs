#[macro_use]
extern crate hippo_api;

#[macro_use]
extern crate rute;

//use std::path::Path;
//use std::ffi::OsStr;
use rute::rute::{DragEnterEvent, DropEvent, ListWidget};
use rute::PluginUi;

use hippo_api::view::View;
use hippo_api::{Service, MessageApi, MessageDecode};
use hippo_api::messages::{AddUrls, ListPosition};

#[derive(Default)]
struct Playlist {
	message_api: MessageApi,
    ui: PluginUi,
    widget: ListWidget,
}

impl View for Playlist {
    fn new(service: &Service) -> Playlist {
    	Playlist {
			message_api: service.message_api(),
			.. Playlist::default()
		}
    }

    fn event(&mut self, msg: &MessageDecode) {
    	println!("msg decode {}", msg.get_method());
    }

    fn setup_ui(&mut self, ui: PluginUi) {
        let vbox = ui.create_v_box_layout();
        let widget = ui.create_list_widget();

        widget.set_drag_enabled(true);
        widget.set_accept_drops(true);
        widget.set_drop_indicator_shown(true);

        set_drag_enter_event!(widget, self, Playlist, Playlist::drag_enter);
        set_drop_event!(widget, self, Playlist, Playlist::drop_files);

        vbox.add_widget(&widget);

        let parent = ui.get_parent();

        parent.set_layout(&vbox);
        parent.resize(500, 500);

        self.ui = ui;
        self.widget = widget;
    }

    fn destroy(&mut self) {
        println!("Destroy plugin");
    }
}

impl Playlist {
	/*
    fn get_filename_only(filename: &str) -> Option<&str> {
        Path::new(filename).file_name().and_then(OsStr::to_str)
    }
    */

    ///
    /// This happens when the user has made a drag'n'drop operation
    /// and added some files/urls to the playlist.
    ///
    /// We don't display them directly but instead send a message to the
    /// host application that will reply back with the files.
    ///
    /// Note: if this turns out to be too slow we should just add them
    ///       as is and update when the updates comes back
    ///
    fn drop_files(&mut self, event: &DropEvent) {
    	let mut urls = Vec::new();

        for url in event
            .mime_data()
            .urls()
            .iter()
            .filter(|u| u.is_local_file())
        {
        	// TODO: Can we use ref here instead? Would save some allocs/copies
        	urls.push(url.to_local_file().to_owned())
        }

        let _add_request_id = self.message_api.add_urls(AddUrls {
        	list_position: ListPosition::End,
        	urls,
        });

        event.accept_proposed_action();
    }

    /*
    fn add_file(&mut self, local_file: &str) {
        let ui = self.ui;

        let mut add_files_message = AddUrls::new(); 



        if let Some(filename) = Self::get_filename_only(&local_file) {
            let item = ui.create_list_widget_item();

            // Check if we already have this file added then we can use the title again
            // TODO: Request song name for this file
            item.set_text(filename);
            item.set_string_data(&local_file);

            self.widget.add_item(&item);
        } else {
            println!(
                "Skipped adding {} as no proper filename was found",
                local_file
            );
        }
    }
    */

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
