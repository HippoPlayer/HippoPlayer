use std::fs::File;
use std::io::{BufWriter, BufReader};
use std::io;

use serde_json;

///
/// Metadata for each entry. We will likely stuff more things here later on.
///
#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct PlaylistEntry {
    path: String,
    title: String,
    duration: f32,
}

#[derive(Serialize, Deserialize, Debug, Default)]
pub struct Playlist {
    entries: Vec<PlaylistEntry>,
    current_song: usize,
}


impl Playlist {
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
}


