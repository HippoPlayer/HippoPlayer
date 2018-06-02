#[macro_use]
extern crate hippo_api;

#[macro_use]
extern crate rute;

extern crate rmp_serde as rmps;
extern crate serde;

//use serde::{Deserialize};
//use rmps::{Deserializer};

use rute::rute::{RUBase, DragEnterEvent, DropEvent, ListWidget, ListWidgetItem};
use rute::PluginUi;
use std::ffi::OsStr;
use std::path::Path;

use hippo_api::messages::{AddUrls, ListPosition};
use hippo_api::view::View;
use hippo_api::{MessageApi, MessageDecode, Service};

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
            ..Playlist::default()
        }
    }

    fn event(&mut self, msg: &mut MessageDecode) {
        println!("event {}", msg.method);

        match msg.method {
            "hippo_playlist_added_files" => self.add_files(msg),
            "hippo_playlist_select_song" => self.select_song(msg),
            "hippo_playlist_load" => self.playlist_loaded(msg),
            _ => (),
        }
    }

    fn setup_ui(&mut self, ui: PluginUi) {
        let vbox = ui.create_v_box_layout();
        let widget = ui.create_list_widget();

        widget.set_drag_enabled(true);
        widget.set_accept_drops(true);
        widget.set_drop_indicator_shown(true);

        set_drag_enter_event!(widget, self, Playlist, Playlist::drag_enter);
        set_drop_event!(widget, self, Playlist, Playlist::drop_files);
        set_list_widget_item_double_clicked_event!(widget, self, Playlist, Playlist::clicked_select_song);

        vbox.add_widget(&widget);

        let parent = ui.get_parent();

        parent.set_layout(&vbox);
        parent.resize(500, 500);

        self.ui = ui;
        self.widget = widget;
    }

    fn destroy(&mut self) {}
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
    fn add_files(&mut self, msg: &mut MessageDecode) {
        let files: AddUrls = msg.deserialize().unwrap();

        for file in &files.urls {
            let item = self.ui.create_list_widget_item();
            let name = Self::get_filename_only(&file).unwrap_or(&file);

            item.set_text(name);
            item.set_string_data(&file);

            self.widget.add_item(&item);
        }
    }

    ///
    /// This handles the message that gets sent to select an new song. This can be a reply from a
    /// when doing a double click event or an external call.
    ///
    fn select_song(&mut self, msg: &mut MessageDecode) {
        let offset: u32 = msg.read_int().unwrap();
        let _url = msg.read_str().unwrap();

        // TODO: validate that this is actually correct
        self.widget.set_current_row(offset as i32);
    }

    ///
    /// Called when a new playlist is loaded. This means that we should clear all old entries
    /// and start filling it with the new data
    ///
    fn playlist_loaded(&mut self, msg: &mut MessageDecode) {
    	self.widget.clear();

    	let len = msg.read_map_len().unwrap();

    	for _ in 0..len {
    	    let item = self.ui.create_list_widget_item();

    	    let path = msg.read_str().unwrap();
    	    let title = msg.read_str().unwrap();

            item.set_string_data(path);
            item.set_text(title);

            self.widget.add_item(&item);
        }
    }

    ///
    /// User double clicks an item in the playlist. Double click means start playing the selected
    /// track and a notification will be sent to start doing so.
    ///
    fn clicked_select_song(&mut self, item: &ListWidgetItem) {
        let url_name = item.get_string_data();
        let row = self.widget.current_row();

        let mut message = self.message_api.begin_request("hippo_playlist_select_song").unwrap();

        message.write_int(row).unwrap();
        message.write_str(&url_name).unwrap();

        self.message_api.end_message(message);
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

        let mut msg = self
            .message_api
            .begin_request("hippo_playlist_add_urls")
            .unwrap();

        msg.serialize(&AddUrls {
            list_position: ListPosition::End,
            urls,
        }).unwrap();

        self.message_api.end_message(msg);

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
