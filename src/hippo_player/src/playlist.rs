use rmp_rpc::message::{Request, Response};
use serde_json;
use std::fs::File;
use std::io::{BufWriter, BufReader};
use std::io;

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

    pub fn handle_message(request: &Request) -> Option<Response> {
        /*
        match request.method {
            "hippo_playlist_get_urls" => {
                None
            }

            _ => None,
        }
        */

        None
    }

    ///
    /// Get the next song to play
    ///
    pub fn get_next_song(&mut self) -> Option<&str> {
        /*
        let count = self.entries.len();

        let mut current_item = self.current_item + 1;

        if current_item >= count  {
            current_item = 0;
        }

        self.current_item = current_item;

        Some(&self.entries[current_item].path)
        */

        None
    }
}


