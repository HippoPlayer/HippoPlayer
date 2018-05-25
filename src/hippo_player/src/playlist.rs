use messages::{AddUrls, LoadedUrls, MessageDecode, MessageEncode};
use std::io;
use std::fs::File;
use std::io::{BufWriter, BufReader};
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
    pub fn event(&mut self, msg: &mut MessageDecode) -> Option<MessageEncode> {
        match msg.method {
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

    pub fn get_loaded_urls(&self) -> MessageEncode {
    	let mut data = LoadedUrls::new();
		let mut request = MessageEncode::new_request("hippo_playlist_load", 0).unwrap();

		data.urls = Vec::with_capacity(self.entries.len());

		for entry in &self.entries {
			data.urls.push((entry.path.clone(), entry.title.clone()));
		}

        request.serialize(&data).unwrap();
		request  
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

    /*
    fn get_prev_song(&mut self) -> Option<&str> {
        let count = self.entries.len();

        let mut current_item = self.current_item + 1;

        if current_item >= count  {
            current_item = 0;
        }

        self.current_item = current_item;

        Some(&self.entries[current_item].path)

        None
    }
    */
}
