use std::collections::HashMap;

pub struct SongDb {
    songs: HashMap<String, Box<[u8]>>,
}

impl SongDb {
    pub fn new() -> SongDb {
        SongDb {
            songs: HashMap::new(),
        }
    }

    pub fn set_data(&mut self, resource: &str, entry: Box<[u8]>) {
        println!("song_db: insterting data {} with size {}", resource, entry.len());
        self.songs.insert(resource.to_owned(), entry);
    }

    pub fn get_data(&self, resource: &str) -> Option<&Box<[u8]>> {
        self.songs.get(resource)
    }
}
