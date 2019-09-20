use serde_derive::{Deserialize, Serialize};
use serde_json;
use std::fs::File;
use std::io;
use std::io::{BufReader, BufWriter};
use messages::HippoMessage;

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
    ///
    ///
    pub fn new() -> Playlist {
        Playlist {
            entries: Vec::new(),
            current_song: 0,
        }
    }

    ///
    /// Handle incoming events
    ///
    pub fn event(&mut self, _msg: &HippoMessage) {
        /*
        match msg.method {
            "hippo_playlist_select_song" => self.select_song(msg),

            /*
            "hippo_playlist_next_song" => {
                self.get_next_song().map(|song| Self::new_song_message(msg, song))
            },

            "hippo_playlist_previous_song" => {
                self.get_prev_song().map(|song| Self::new_song_message(msg, song))
            },
            */
            "hippo_playlist_add_urls" => {
                let files: AddUrls = msg.deserialize().unwrap();

                for file in &files.urls {
                    let entry = PlaylistEntry {
                        path: file.clone(),
                        title: file.clone(),
                        duration: -10.0,
                    };

                    self.entries.push(entry);
                }

                let mut message =
                    MessageEncode::new_request("hippo_playlist_added_files", msg.id).unwrap();
                message.serialize(&files).unwrap();

                Some(message)
            }

            _ => None,
        }
        */
    }

    ///
    /// Serialize the playlist and write it to filename
    ///
    pub fn save(&mut self, filename: &str) -> io::Result<()> {
        let f = BufWriter::new(File::create(filename)?);
        serde_json::to_writer_pretty(f, self)?;
        Ok(())
    }

    ///
    /// Load the playlist
    ///
    pub fn load(&mut self, filename: &str) -> io::Result<()> {
        let f = BufReader::new(File::open(filename)?);
        *self = serde_json::from_reader(f)?;
        Ok(())
    }

    ///
    /// Get loaded urls
    ///
    pub fn get_loaded_urls(&self) /*-> MessageEncode*/ {
        /*
        let mut request = MessageEncode::new_request("hippo_playlist_load", 0).unwrap();

        request.write_map_len(self.entries.len() as u32).unwrap();

        for entry in &self.entries {
            request.write_str(&entry.path).unwrap();
            request.write_str(&entry.title).unwrap();
        }

        request
        */
    }

    ///
    /// Get the next song to play
    ///
    fn select_song(&mut self, msg: &HippoMessage)  {
        /*
        let row: u32 = msg.read_int().unwrap();
        let url_name = msg.read_str().unwrap();

        // TODO: Validate that this index is correct

        self.current_song = row as usize;

        // Write reply

        let mut reply = MessageEncode::new_request("hippo_playlist_select_song", 0).unwrap();
        reply.write_int(row).unwrap();
        reply.write_str(&url_name).unwrap();

        Some(reply)
        */
    }

    ///
    /// Get the current song
    ///
    pub fn get_current_song(&mut self) -> Option<String> {
        if !self.entries.is_empty() {
            Some(self.entries[self.current_song].path.to_owned())
        } else {
            None
        }
    }

    ///
    /// Get the next song to play
    ///
    pub fn get_next_song(&mut self) -> Option<String> {
        let count = self.entries.len();

        if count == 0 {
            return None;
        }

        let current_song = (self.current_song + 1) % count;
        self.current_song = current_song;

        Some(self.entries[current_song].path.to_owned())
    }

    ///
    /// Get previous song
    ///
    fn get_prev_song(&mut self) -> Option<String> {
        let count = self.entries.len();

        let mut current_song = self.current_song + 1;

        if current_song >= count {
            current_song = 0;
        }

        self.current_song = current_song;

        Some(self.entries[current_song].path.to_owned())
    }
}
