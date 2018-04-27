use std::collections::HashMap;
use std::path::Path;
use std::ffi::OsStr;
use wrui::wrui::{ListWidgetItem, ListWidget, DragEnterEvent, DropEvent};
use wrui::Ui;
use audio::MusicInfo;

use std::os::raw::c_void;

use playlist::PlaylistEntry;

pub struct PlaylistView {
    pub widget: ListWidget,
    playlist_data: HashMap<String, PlaylistEntry>,
    wrui: Ui,
}

fn get_filename_only(filename: &str) -> Option<&str> {
    Path::new(filename).file_name().and_then(OsStr::to_str)
}

///
/// Displays and handles playlist
///
impl PlaylistView {
    pub fn new(wrui: Ui) -> PlaylistView {
        PlaylistView {
            playlist_data: HashMap::new(),
            widget: wrui.create_list_widget(),
            wrui,
        }
    }

    ///
    ///
    ///
    pub fn setup(&mut self) {
        // Enable drag'n'drop on playlist

        self.widget.set_drag_enabled(true);
        self.widget.set_accept_drops(true);
        self.widget.set_drop_indicator_shown(true);

        set_drag_enter_event!(self.widget, self, PlaylistView, PlaylistView::drag_enter);
        set_drop_event!(self.widget, self, PlaylistView, PlaylistView::drop_files);
    }

    fn drag_enter(&mut self, event: &DragEnterEvent) {
        event.accept();
    }

    pub fn select_song(&mut self, item: &ListWidgetItem, music_info: &MusicInfo) {
        /*
        if music_info.title != "" {
            item.set_text(&music_info.title);
        }

        if let Some(lookup) = self.playlist_data.get_mut(&item.get_string_data()) {
            if music_info.title == "" {
                lookup.title = music_info.title.clone();
            } else {
                lookup.title = item.text();
            }

            lookup.duration = music_info.duration as f32;
        }
        */
    }

    pub fn add_file(&mut self, _local_file: &str) {
        /*
        if let Some(filename) = get_filename_only(&local_file) {
            let item = self.wrui.create_list_widget_item();

            // Check if we already have this file added then we can use the title again

            if let Some(entry) = self.playlist_data.get(local_file) {
                item.set_text(&entry.title);
            } else {
                item.set_text(filename);
            }

            item.set_string_data(&local_file);

            self.widget.add_item(&item);
        } else {
            println!("Skipped adding {} as no proper filename was found", local_file);
        }
        */
    }

    fn drop_files(&mut self, event: &DropEvent) {
        for url in event.mime_data().urls().iter().filter(|u| u.is_local_file()) {
            self.add_file(&url.to_local_file());
        }

        event.accept_proposed_action();
    }

    ///
    /// Get the next song to play
    ///
    pub fn get_next_song(&mut self) -> Option<ListWidgetItem> {
        let total_count = self.widget.count();
        let mut current_item = self.widget.current_row() + 1;

        if current_item >= total_count {
            current_item = 0;
        }

        self.widget.set_current_row(current_item);
        self.widget.current_item()
    }

    ///
    /// Get the previous song to play
    ///
    pub fn get_prev_song(&mut self) -> Option<ListWidgetItem> {
        let total_count = self.widget.count();
        let mut current_item = self.widget.current_row() - 1;

        if current_item < -1 {
            current_item = total_count - 1;
        }

        self.widget.set_current_row(current_item);
        self.widget.current_item()
    }
}
