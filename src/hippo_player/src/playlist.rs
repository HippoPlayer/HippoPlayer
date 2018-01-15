use std::collections::HashMap;
use std::fs::File;
use std::io::{BufWriter, BufReader};
use std::path::Path;
use std::ffi::OsStr;
use std::io;
use wrui::wrui::{ListWidgetItem, ListWidget, DragEnterEvent, DropEvent};
use wrui::Ui;
use audio::MusicInfo;

use std::os::raw::c_void;

use serde_json;

///
/// Metadata for each entry. We will likely stuff more things here later on.
///
#[derive(Serialize, Deserialize, Clone, Debug)]
struct PlaylistEntry {
    path: String,
    title: String,
    duration: f32,
}

#[derive(Serialize, Deserialize, Debug, Default)]
struct PlaylistData {
    entries: Vec<PlaylistEntry>,
    current_song: usize,
}

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

    ///
    /// Serialize the playlist and write it to filename
    ///
    pub fn save_playlist(&mut self, filename: &str) -> io::Result<()> {
        let mut playlist = PlaylistData {
            entries: Vec::new(),
            current_song: self.widget.current_row() as usize,
        };

        for index in 0..self.widget.count() {
            if let Some(entry) = self.widget.item(index) {
                // we store the path to the file in the string data and use that as the key
                let path = entry.get_string_data();

                if let Some(entry) = self.playlist_data.get(&path) {
                    playlist.entries.push(entry.clone());
                } else {
                    // This path should never happen
                    playlist.entries.push(PlaylistEntry {
                        path: path.clone(),
                        title: entry.text(),
                        duration: -10.0,
                    });
                }
            }
        }

        let f = BufWriter::new(File::create(filename)?);
        serde_json::to_writer_pretty(f, &playlist)?;

        Ok(())
    }

    fn drag_enter(&mut self, event: &DragEnterEvent) {
        event.accept();
    }

    pub fn select_song(&mut self, item: &ListWidgetItem, music_info: &MusicInfo) {
        item.set_text(&music_info.title);

        if let Some(lookup) = self.playlist_data.get_mut(&item.get_string_data()) {
            lookup.title = music_info.title.clone();
            lookup.duration = music_info.duration as f32;
        }
    }

    fn drop_files(&mut self, event: &DropEvent) {
        for url in event.mime_data().urls().iter().filter(|u| u.is_local_file()) {
            let local_file = url.to_local_file();

            if let Some(filename) = get_filename_only(&local_file) {
                let item = self.wrui.create_list_widget_item();

                // Check if we already have this file added then we can use the title again

                if let Some(entry) = self.playlist_data.get(&local_file) {
                    item.set_text(&entry.title);
                } else {
                    item.set_text(filename);
                }

                item.set_string_data(&local_file);

                self.widget.add_item(&item);
            } else {
                println!("Skipped adding {} as no proper filename was found", local_file);
            }
        }

        event.accept_proposed_action();
    }

    ///
    /// Load the playlist
    ///
    pub fn load_playlist(&mut self, filename: &str) -> io::Result<()> {
        let f = BufReader::new(File::open(filename)?);
        let playlist: PlaylistData = serde_json::from_reader(f)?;

        self.playlist_data.clear();

        for item in &playlist.entries {
            self.playlist_data.insert(item.path.clone(), item.clone());

            let list_item = self.wrui.create_list_widget_item();
            list_item.set_text(&item.title);
            list_item.set_string_data(&item.path);

            self.widget.add_item(&list_item);
        }

        self.widget.set_current_row(playlist.current_song as i32);

        Ok(())
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
}
