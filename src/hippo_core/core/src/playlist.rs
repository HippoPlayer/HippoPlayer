use serde_derive::{Deserialize, Serialize};
use serde_json;
use std::fs::File;
use std::io;
use std::io::{BufReader, BufWriter};
use messages::{HippoMessage, MessageType};

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
    current_song: isize,
    new_song: bool,
}

impl Playlist {
    ///
    ///
    ///
    pub fn new() -> Playlist {
        Playlist {
            entries: Vec::new(),
            current_song: 0,
            new_song: false,
        }
    }

    ///
    /// Advance a song in the songlist
    ///
    fn advance_song(&mut self, direction: isize) {
        let count = self.entries.len() as isize;

        if count > 0 {
            let new_song = (self.current_song + direction) % count;

            if new_song != self.current_song {
                self.new_song = true;
            }

            self.current_song = new_song;
        }
    }

    ///
    /// Handle incoming events
    ///
    pub fn event(&mut self, msg: &HippoMessage) -> Option<Box<[u8]>> {
        match msg.message_type() {
            MessageType::add_urls => {
                let urls = msg.message_as_add_urls().unwrap();

                println!("data {:?}", urls.urls());
            },

            MessageType::next_song => self.advance_song(1),
            MessageType::prev_song => self.advance_song(-1),

            _ => (),
        }

        None


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
    fn select_song(&mut self, _msg: &HippoMessage)  {
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
    /// Check if current song has been updated and change the flag to false
    ///
    pub fn is_current_song_updated(&mut self) -> bool {
        let new_song = self.new_song;
        self.new_song = false;
        new_song
    }

    ///
    /// Get the current song
    ///
    pub fn get_current_song(&mut self) -> Option<String> {
        if !self.entries.is_empty() {
            Some(self.entries[self.current_song as usize].path.to_owned())
        } else {
            None
        }
    }
}
