use messages::*;
use serde_derive::{Deserialize, Serialize};
use serde_json;
use std::fs::File;
use std::io;
use std::io::{BufReader, BufWriter};
use std::path::Path;
use song_db::SongDb;

///
/// Metadata for each entry. We will likely stuff more things here later on.
///
#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct PlaylistEntry {
    path: String,
    title: String,
    duration: f32,
    song_type: String,
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
    pub fn advance_song(&mut self, direction: isize) -> Option<Box<[u8]>> {
        let count = self.entries.len() as isize;

        if count > 0 {
            let new_song = (self.current_song + direction) % count;

            if new_song != self.current_song {
                self.new_song = true;
            }

            self.current_song = new_song;
        }

        None

        //self.current_song_message()
    }

    pub fn update_current_entry(&mut self, song_db: &SongDb, url: &str) {
        let entry = &mut self.entries[self.current_song as usize];

        song_db.get_tag("title", url).map(|title| {
            if title != "" {
                entry.title = title;
            }
        });

        entry.duration = song_db.get_tag_f64("length", url).unwrap() as f32;
        song_db
            .get_tag("song_type", url)
            .map(|song_type| entry.song_type = song_type);
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
                let song_type = builder.create_string(&entry.song_type);

                let song_desc = HippoSongDescription::create(
                    &mut builder,
                    &HippoSongDescriptionArgs {
                        authoring_tool: None,
                        artist: None,
                        date: None,
                        title: Some(title_name),
                        song_type: Some(song_type),
                        duration: entry.duration,
                    },
                );

                out_ent.push(HippoUrlEntry::create(
                    &mut builder,
                    &HippoUrlEntryArgs {
                        path: Some(path_name),
                        description: Some(song_desc),
                    },
                ));
            }

            let urls_vec = builder.create_vector(&out_ent);

            let added_urls = HippoReplyAddedUrls::create(
                &mut builder,
                &HippoReplyAddedUrlsArgs {
                    urls: Some(urls_vec),
                },
            );

            Some(HippoMessage::create_def(
                builder,
                MessageType::reply_added_urls,
                added_urls.as_union_value(),
            ))
        }
    }

    ///
    ///
    ///
    pub fn current_song_message(&self) -> Option<Box<[u8]>> {
        if self.entries.is_empty() {
            return None;
        }

        let entry = &self.entries[self.current_song as usize];

        let mut builder = messages::FlatBufferBuilder::new_with_capacity(8192);
        let title = builder.create_string(&entry.title);
        let song_type = builder.create_string(&entry.song_type);

        let song_desc = HippoSongDescription::create(
            &mut builder,
            &HippoSongDescriptionArgs {
                authoring_tool: None,
                artist: None,
                date: None,
                title: Some(title),
                song_type: Some(song_type),
                duration: entry.duration,
            },
        );

        let select_song = HippoSelectSong::create(
            &mut builder,
            &HippoSelectSongArgs {
                description: Some(song_desc),
                playlist_index: self.current_song as i32,
            },
        );

        Some(HippoMessage::create_def(
            builder,
            MessageType::select_song,
            select_song.as_union_value(),
        ))
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
                            song_type: String::new(),
                            duration: 0.0,
                        });
                    }
                }

                self.create_update_message(index_start)
            }

            MessageType::request_select_song => self.select_song(msg),
            MessageType::next_song => self.advance_song(1),
            MessageType::prev_song => self.advance_song(-1),
            MessageType::request_added_urls => self.create_update_message(0),
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
    /// Get loaded urls
    ///
    /*
    pub fn get_loaded_urls(&self) -> Option<Box<[u8]>> {
        self.create_update_message(0)
    }
    */

    ///
    /// Select a new song to play
    ///
    fn select_song(&mut self, msg: &HippoMessage) -> Option<Box<[u8]>> {
        let mut new_song_id = None;
        let select_song = msg.message_as_request_select_song().unwrap();
        let path_name = select_song.path().unwrap();
        let playlist_index = select_song.playlist_index() as usize;

        if playlist_index < self.entries.len() {
            new_song_id = Some(playlist_index);
        } else {
            println!("Warning: Tried to select song at {} - {} but not enough entries in the playlist {}",
                playlist_index, path_name, self.entries.len());
        }

        if let Some(id) = new_song_id {
            self.current_song = id as isize;
            self.new_song = true;
        }

        None
    }

    ///
    /// Remove a entry from the list
    ///
    pub fn remove_entry(&mut self, entry: i32) {
        self.entries.remove(entry as usize);
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
