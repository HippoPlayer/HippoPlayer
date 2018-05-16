use serde_json;
use std::fs::File;
use std::io::{BufWriter, BufReader};
use std::io;
use service::{MessageDecode, MessageEncode, REQUEST_MESSAGE};
use hippo_api::messages::AddUrls; 
use serde::{Serialize, Deserialize};
use rmps::{Serializer, Deserializer};

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

    fn new_song_message(incoming_msg: &MessageDecode, filename: &str) -> MessageEncode {
        let mut message = MessageEncode::new(incoming_msg.notifaction_id, REQUEST_MESSAGE);

        message.write_array_len(4).unwrap();
        message.write_uint(REQUEST_MESSAGE).unwrap();
        message.write_uint(incoming_msg.notifaction_id as u64).unwrap();
        message.write_str("hippo_playlist_select_song").unwrap();
        message.write_str(filename).unwrap();
        message
    }

    ///
    /// Handle incoming events 
    /// 
	pub fn event(&mut self, msg: &MessageDecode) -> Option<MessageEncode> {
		match msg.get_method().as_ref() {
			/*
			"hippo_playlist_next_song" => {
				self.get_next_song().map(|song| Self::new_song_message(msg, song))
			},

			"hippo_playlist_previous_song" => {
				self.get_prev_song().map(|song| Self::new_song_message(msg, song))
			},
			*/

			"hippo_playlist_add_urls" => {
				println!("deserialize files {}", msg.cursor.position());

				let mut de = Deserializer::new(msg.cursor.clone());
				let files: AddUrls = Deserialize::deserialize(&mut de).unwrap();

				for file in &files.urls {
					let entry = PlaylistEntry {
						path: file.clone(),
						title: file.clone(),
						duration: -10.0,
					};

					self.entries.push(entry);
				}

				let mut message = MessageEncode::new(msg.notifaction_id, REQUEST_MESSAGE);
				message.write_array_len(4).unwrap();
				message.write_uint(REQUEST_MESSAGE).unwrap();
				message.write_uint(msg.notifaction_id as u64).unwrap();
				message.write_str("hippo_playlist_added_files").unwrap();
				files.serialize(&mut Serializer::new(&mut message)).unwrap();

				Some(message)
			},

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

    fn get_prev_song(&mut self) -> Option<&str> {
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


