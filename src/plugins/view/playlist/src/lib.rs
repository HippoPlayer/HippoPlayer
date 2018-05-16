#[macro_use]
extern crate hippo_api;

#[macro_use]
extern crate rute;

extern crate serde;
extern crate rmp_serde as rmps;

use serde::{Deserialize};
use rmps::{Deserializer};

use std::path::Path;
use std::ffi::OsStr;
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
    	match msg.get_method().as_ref() {
    		"hippo_playlist_added_files" => self.add_files(&msg),
    		_ => (),
    	}

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
	// TODO: Correct names should be sent to added_files instead
    fn get_filename_only(filename: &str) -> Option<&str> {
        Path::new(filename).file_name().and_then(OsStr::to_str)
    }

	///
	/// Message sent when files needs to be added to the actual view.
	/// It's usually the playlist that sends this event
	/// 
	fn add_files(&mut self, msg: &MessageDecode) {
		let mut de = Deserializer::new(msg.get_raw_data());
		let files: AddUrls = Deserialize::deserialize(&mut de).unwrap();

		for file in &files.urls {
			let item = self.ui.create_list_widget_item();
        	let name = Self::get_filename_only(&file).unwrap_or(&file);

			item.set_text(name);
			item.set_string_data(&file);

			self.widget.add_item(&item);
		}
	}

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
