use std::collections::HashMap;

#[derive(Default, Debug)]
struct SongInfo {
    data: HashMap<String, String>,
}

#[derive(Default, Debug)]
struct SongEntry {
    song: SongInfo,
    sub_songs: HashMap<usize, SongInfo>,
}

pub struct SongDb {
    songs: HashMap<String, SongEntry>,
}

impl SongDb {
    pub fn new() -> SongDb {
        SongDb {
            songs: HashMap::new()
        }
    }

    fn update_song_info(entry: &mut SongInfo, key: &str, value: &str) {
        entry.data.insert(key.to_owned(), value.to_owned());
    }

    fn update_entry(entry: &mut SongEntry, sub_song: usize, value: &str, key: &str) {
        if sub_song == 0 {
            Self::update_song_info(&mut entry.song, key, value);
        } else {
            // Fix: NLL
            let mut updated_entry = false;

            if let Some(song_info) = entry.sub_songs.get_mut(&sub_song) {
                updated_entry = true;
                Self::update_song_info(song_info, key, value);
            }

            if !updated_entry {
                let mut song_info = SongInfo::default();
                Self::update_song_info(&mut song_info, key, value);
                entry.sub_songs.insert(sub_song, song_info);
            }
        }
    }

    pub fn set_key(&mut self, resource: &str, sub_song: usize, value: &str, key: &str) {
        // Fix: NLL
        println!("Set key {} - {} - {} - {}", resource, sub_song, value, key);

        let mut updated_entry = false;

        if let Some(entry) = self.songs.get_mut(resource) {
            updated_entry = true;
            Self::update_entry(entry, sub_song, value, key);
        }

        if !updated_entry {
            let mut entry = SongEntry::default();
            Self::update_entry(&mut entry, sub_song, value, key);
            self.songs.insert(resource.to_owned(), entry);
        }
    }

    pub fn get_key(&self, resource: &str, sub_song: usize, key: &str) -> Option<String> { 
        match self.songs.get(resource) {
            None => None,
            Some(entry) => {
                if sub_song > 0 {
                    entry.sub_songs.get(&sub_song).and_then(|t| t.data.get(key)).and_then(|v| Some((*v).to_string()))
                } else {
                    entry.song.data.get(key).and_then(|v| Some((*v).to_string()))
                }
            }
        }
    }
}
