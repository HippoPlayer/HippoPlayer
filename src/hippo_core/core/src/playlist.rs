use serde_derive::{Deserialize, Serialize};
use serde_json;
use std::fs::File;
use std::io;
use std::io::{BufReader, BufWriter};
use messages::*;
use std::path::Path;

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
    /// Used if the playlist has been updated or just loaded.
    ///
    pub fn create_update_message(&self, index_start: usize) -> Option<Box<[u8]>> {
        let count = self.entries.len();

        if index_start == count {
            None
        } else {
            let entries = &self.entries[index_start..count];
            let mut builder = messages::FlatBufferBuilder::new_with_capacity(8192);
            let mut out_ent = Vec::with_capacity(count - index_start);

            for entry in entries {
                let path_name = builder.create_string(&entry.path);
                let title_name = builder.create_string(&entry.title);

                out_ent.push(HippoUrlEntry::create(&mut builder, &HippoUrlEntryArgs {
                    path: Some(path_name),
                    title: Some(title_name),
                    length: entry.duration
                }));
            }

            let urls_vec = builder.create_vector(&out_ent);

            let added_urls = HippoReplyAddedUrls::create(&mut builder, &HippoReplyAddedUrlsArgs {
                urls: Some(urls_vec),
            });

            Some(HippoMessage::create_def(builder, MessageType::reply_added_urls, added_urls.as_union_value()))
        }
    }

    ///
    /// Handle incoming events
    ///
    pub fn event(&mut self, msg: &HippoMessage) -> Option<Box<[u8]>> {
        match msg.message_type() {
            MessageType::request_add_urls => {
                // TODO: Currently we just assume these are filenames. Going forward
                // This may not be the case as it may be a network resource, packed files and such

                let index_start = self.entries.len();

                let urls_msg = msg.message_as_request_add_urls().unwrap();
                let urls = urls_msg.urls().unwrap();

                for index in 0..urls.len() {
                    let file = urls.get(index);

                    // Assume files now and if we can't create a file from the path we drop it
                    let filename = Path::new(&file);
                    if let Some(base) = filename.file_stem() {
                        self.entries.push(PlaylistEntry {
                            path: file.to_owned(),
                            title: base.to_string_lossy().to_string(),
                            duration: -1.0, // negative is used to indicate no durion
                        });
                    }
                }

                return self.create_update_message(index_start);
            },

            MessageType::request_select_song => return self.select_song(msg),
            MessageType::next_song => self.advance_song(1),
            MessageType::prev_song => self.advance_song(-1),
            MessageType::request_added_urls => return self.create_update_message(0),

            _ => (),
        }

        None
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
    pub fn get_loaded_urls(&self) -> Option<Box<[u8]>> {
        self.create_update_message(0)
    }

    ///
    /// Select a new song to play 
    ///
    fn select_song(&mut self, msg: &HippoMessage) -> Option<Box<[u8]>>  {
        // This is kinda ugly but will do for now
        // TODO: Proper error handling

        let select_song = msg.message_as_request_select_song().unwrap();
        let song_name = select_song.name().unwrap();
        let mut new_song_id = None;

        for (id, entry) in self.entries.iter().enumerate() {
            if entry.path == song_name {
                new_song_id = Some(id);
                break;
            }
        }

        if let Some(id) = new_song_id {
            self.current_song = id as isize;
            self.new_song = true;

            let mut builder = messages::FlatBufferBuilder::new_with_capacity(8192);
            let title = builder.create_string(&self.entries[id as usize].title);

            let select_song = HippoSelectSong::create(&mut builder, &HippoSelectSongArgs {
                title: Some(title),
            });

            Some(HippoMessage::create_def(builder, MessageType::select_song, select_song.as_union_value()))
        } else {
            None
        }
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
