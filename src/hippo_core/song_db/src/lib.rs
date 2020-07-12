use sqlite::{Error, State};


const CREATE_URLS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS urls(
            pk INTEGER PRIMARY KEY DESC,
            path STRING,
            title STRING,
            song_type STRING,
            authoring_tool STRING,
            artist STRING,
            album STRING,
            date STRING,
            genre STRING,
            message STRING,
            length STRING,
            ripper STRING,
            extra_data BLOB)";

const CREATE_SUB_SONGS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS sub_songs(
            url_id INTEGER,
            sub_index INTEGER,
            title STRING,
            length STRING,
            extra_data BLOB)";

const CREATE_SAMPLES_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS samples(
            url_id INTEGER,
            text STRING)";

const CREATE_INSTRUMENTS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS instruments(
            url_id INTEGER,
            text STRING)";

pub struct SongDb {
    /// sqlite database
    pub connection: sqlite::Connection,
    /// Caching when getting metadata
    pub query_data: Vec<String>,
    pub cached_samples: Vec<String>,
    pub cached_instruments: Vec<String>,
}

impl SongDb {
    ///
    /// Open db and/or create the needed tables if they don't exist
    ///
    pub fn new(path: &str) -> Result<SongDb, Error> {
        let connection = sqlite::open(path)?;
        let mut query_data = Vec::with_capacity(1024);
        let mut cached_samples = Vec::with_capacity(1024);
        let mut cached_instruments = Vec::with_capacity(1024);

        // TODO: Better way to do this
        for _ in 0..1024 {
            query_data.push(String::new());
            cached_samples.push(String::new());
            cached_instruments.push(String::new());
        }

        connection.execute(CREATE_URLS_TABLE)?;
        connection.execute(CREATE_SUB_SONGS_TABLE)?;
        connection.execute(CREATE_SAMPLES_TABLE)?;
        connection.execute(CREATE_INSTRUMENTS_TABLE)?;
        connection.execute("PRAGMA journal_mode=WAL")?; // for less trafic to disk
        Ok(SongDb { connection, query_data, cached_samples, cached_instruments })
    }

    pub fn begin_transaction(&self) {
        if let Err(e) = self.connection.execute("BEGIN TRANSACTION;") {
            println!("begin transacation failed: {}", e);
        }
    }

    pub fn commit(&self) {
        if let Err(e) = self.connection.execute("COMMIT;") {
            println!("commit failed: {}", e);
        }
    }

    ///
    /// Create an url and insert it into the urls table
    ///
    pub fn create_url(&self, url: &str) -> Result<u64, Error> {
        let url_id = xxh3::hash(url.as_bytes());
        let t = format!(
            "INSERT or IGNORE into urls (pk, path) VALUES({}, \"{}\")",
            url_id, url
        );
        self.connection.execute(t)?;
        Ok(url_id)
    }

    pub fn set_tag_string(&self, url_id: u64, key: &str, data: &str) -> Result<(), Error> {
    	let t;
    	if data.contains('"') {
			t = format!(
            	"UPDATE urls SET {} = '{}' where pk == {}",
				key, data, url_id,
			);
    	} else {
			t = format!(
            	"UPDATE urls SET {} = \"{}\" where pk == {}",
				key, data, url_id,
			);
    	}

        self.connection.execute(t)?;
        Ok(())
    }

    pub fn set_tag_float(&self, url_id: u64, key: &str, data: f32) -> Result<(), Error> {
        let t = format!(
            "UPDATE urls SET {} = \"{}\" where pk == {}",
            key, data, url_id
        );
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn add_sub_song(&self, url_id: u64, index: i32, name: &str, length: f64) -> Result<(), Error> {
        let t = format!(
            "INSERT or IGNORE into sub_songs (url_id, sub_index, title, length) VALUES({}, {}, \"{}\", {})",
            url_id, index, name, length
        );
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn add_sample(&self, url_id: u64, sample_text: &str) -> Result<(), Error> {
    	let t;
    	if sample_text.contains('"') {
			t = format!(
				"INSERT into samples (url_id, text) VALUES({}, '{}')",
				url_id, sample_text
			);
		} else {
			t = format!(
				"INSERT into samples (url_id, text) VALUES({}, \"{}\")",
				url_id, sample_text
			);
		}
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn add_instrument(&self, url_id: u64, instrument_text: &str) -> Result<(), Error> {
    	let t;
    	if instrument_text.contains('"') {
			t = format!(
				"INSERT into instruments (url_id, text) VALUES({}, '{}')",
				url_id, instrument_text
			);
    	} else {
			t = format!(
				"INSERT into instruments (url_id, text) VALUES({}, \"{}\")",
				url_id, instrument_text
			);
    	}

        self.connection.execute(t)?;
        Ok(())
    }

    fn get_statement(&self, tag: &str, url: &str) -> sqlite::Statement {
    	// bad hack!
    	if let Some(subsong) = url.find('|') {
			let query;
        	let url_id = xxh3::hash(&url[..subsong].as_bytes());
            let index = *&url[subsong + 1..].parse::<i32>().unwrap();

			// hack
			if tag != "length" && tag != "title" {
				query = format!("SELECT {} FROM urls WHERE pk=={}", tag, url_id);
			} else {
        		query = format!("SELECT {} FROM sub_songs WHERE url_id=={} AND sub_index=={}", tag, url_id, index);
			}
        	self.connection.prepare(query).unwrap()
    	} else {
        	let url_id = xxh3::hash(url.as_bytes());
			let query = format!("SELECT {} FROM urls WHERE pk=={}", tag, url_id);
			self.connection.prepare(query).unwrap()
    	}
    }

    ///
    /// Get tag for a url
    ///
    pub fn get_tag(&self, tag: &str, url: &str) -> Option<String> {
        let mut statement = self.get_statement(tag, url);

        if sqlite::State::Row == statement.next().unwrap() {
            let entry = statement.read::<String>(0).unwrap();
            Some(entry)
        } else {
            None
        }
    }

    ///
    /// Get tag for a url
    ///
    pub fn get_tag_f64(&self, tag: &str, url: &str) -> Option<f64> {
        let mut statement = self.get_statement(tag, url);

        if sqlite::State::Row == statement.next().unwrap() {
            let entry = statement.read::<f64>(0).unwrap();
            Some(entry)
        } else {
        	None
        }
    }

	/// get the has for the url but ignore any subsongs if present
    fn get_base_url_hash(url: &str) -> u64 {
		if let Some(sep) = url.find('|') {
        	xxh3::hash(url[..sep].as_bytes())
		} else {
        	xxh3::hash(url.as_bytes())
		}
    }

    pub fn is_present(&self, url: &str) -> bool {
    	let url_id = Self::get_base_url_hash(url);
        let query = format!("SELECT pk FROM urls WHERE pk=={}", url_id);

        let mut count = 0;
        let mut statement = self.connection.prepare(query).unwrap();

        if sqlite::State::Row == statement.next().unwrap() {
            count = 1;
        }

        count == 1
    }

    pub fn get_subsongs(&self, url: &str) -> Option<Vec<String>> {
        let url_id = xxh3::hash(url.as_bytes());
        let query = format!("SELECT title FROM sub_songs WHERE url_id=={}", url_id);
        let mut statement = self.connection.prepare(query).unwrap();

        let mut subsongs = Vec::new();

		while let State::Row = statement.next().unwrap() {
			subsongs.push(statement.read::<String>(0).unwrap());
		}

		if subsongs.is_empty() { None } else { Some(subsongs) }
    }

    pub fn begin_get_all(&mut self, url: &str) -> u32 {
        let url_id;

        if let Some(subsong_sep) = url.find('|') {
        	url_id = xxh3::hash(&url[..subsong_sep].as_bytes());
        } else {
        	url_id = xxh3::hash(url.as_bytes());
        }

		let query = format!("SELECT * FROM urls WHERE pk=={}", url_id);
        let mut statement = self.connection.prepare(query).unwrap();
        let count = statement.names().len();
        let mut total_count = 0;

        if sqlite::State::Row == statement.next().unwrap() {
            // loop from 1 to skip pk (primary key for the database)
            for i in 1..count {
                match statement.read::<String>(i) {
                    Ok(v) => {
                        self.query_data[(total_count * 2) + 0].clear();
                        self.query_data[(total_count * 2) + 1].clear();
                        self.query_data[(total_count * 2) + 0].push_str(statement.name(i));
                        self.query_data[(total_count * 2) + 1].push_str(&v);
                        total_count += 1;
                    },
                    _ => (),
                }
            }
        }

        {
            let query = format!("SELECT text from samples where url_id={}", url_id);
            let mut statement = self.connection.prepare(query).unwrap();

            // TODO: Optimize
            self.cached_samples.clear();

            while let State::Row = statement.next().unwrap() {
                self.cached_samples.push(statement.read::<String>(0).unwrap());
            }
        }

        {
            let query = format!("SELECT text from instruments where url_id={}", url_id);
            let mut statement = self.connection.prepare(query).unwrap();

            // TODO: Optimize
            self.cached_instruments.clear();

            while let State::Row = statement.next().unwrap() {
                self.cached_instruments.push(statement.read::<String>(0).unwrap());
            }
        }

        total_count as u32
    }

    pub fn get_all_entry_name(&self, index: u32, len: *mut i32) -> *const u8 {
        let bytes = self.query_data[((index as usize) * 2) + 0].as_bytes();
        unsafe { *len = bytes.len() as i32; }
        bytes.as_ptr()
    }

    pub fn end_get_all() {

    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_in_memory_db() {
        SongDb::new(":memory:").unwrap();
    }

    #[test]
    fn test_file_db() {
        {
            SongDb::new("test.db").unwrap();
        }

        std::fs::remove_file("test.db").unwrap();
    }

    #[test]
    fn test_file_db_twice() {
        {
            SongDb::new("test_2.db").unwrap();
        }

        {
            SongDb::new("test_2.db").unwrap();
        }

        std::fs::remove_file("test_2.db").unwrap();
    }

    #[test]
    fn test_create_url() {
        let db = SongDb::new(":memory:").unwrap();
        db.create_url("this/is/some/path").unwrap();
    }

    #[test]
    fn test_create_same_url_twice() {
        let path = "this/is/some/path";
        let db = SongDb::new(":memory:").unwrap();
        db.create_url(path).unwrap();
        db.create_url(path).unwrap();
    }

    #[test]
    fn test_create_query_url() {
        let path = "this/is/some/path";
        let db = SongDb::new(":memory:").unwrap();
        let url_id = db.create_url(path).unwrap();
        let query = format!("SELECT * FROM urls WHERE pk=={}", url_id);

        let mut count = 0;
        // expect to have one query back
        let mut statement = db.connection.prepare(query).unwrap();

        while let sqlite::State::Row = statement.next().unwrap() {
            count += 1;
        }

        assert_eq!(count, 1);
    }

    #[test]
    fn test_add_sub_song() {
        let db = SongDb::new(":memory:").unwrap();
        let url_id = db.create_url("this/is/some/path").unwrap();
        db.add_sub_song(url_id, "test title", 0, 1.0).unwrap();
    }

    #[test]
    fn test_add_sample() {
        let db = SongDb::new(":memory:").unwrap();
        let url_id = db.create_url("this/is/some/path").unwrap();
        db.add_sample(url_id, "test title").unwrap();
    }

    #[test]
    fn test_add_instrument() {
        let db = SongDb::new(":memory:").unwrap();
        let url_id = db.create_url("this/is/some/path").unwrap();
        db.add_instrument(url_id, "instrument").unwrap();
    }

    #[test]
    fn test_set_tag_title() {
        let db = SongDb::new(":memory:").unwrap();
        let my_title = "my title";
        let url_id = db.create_url("this/is/some/path").unwrap();
        db.set_tag_string(url_id, "title", my_title).unwrap();

        let query = format!("SELECT title FROM urls WHERE pk=={}", url_id);

        let mut statement = db.connection.prepare(query).unwrap();

        while let sqlite::State::Row = statement.next().unwrap() {
            assert_eq!(my_title, statement.read::<String>(0).unwrap());
        }
    }

    #[test]
    fn test_set_tag_float() {
        let db = SongDb::new(":memory:").unwrap();
        let url_id = db.create_url("this/is/some/path").unwrap();
        db.set_tag_float(url_id, "length", 2.0).unwrap();

        let query = format!("SELECT length FROM urls WHERE pk=={}", url_id);

        let mut statement = db.connection.prepare(query).unwrap();

        while let sqlite::State::Row = statement.next().unwrap() {
            assert_eq!(2.0, statement.read::<f64>(0).unwrap());
        }
    }

    #[test]
    fn test_is_present() {
        let path = "this/is/some/path";

        let db = SongDb::new(":memory:").unwrap();
        db.create_url(path).unwrap();

        assert_eq!(db.is_present(path), true);
        assert_eq!(db.is_present("not/present"), false);
    }
}
