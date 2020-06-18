use sqlite;
use sqlite::Error;
use xxhrs::XXH3_64;

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
            length REAL,
            ripper STRING,
            extra_data BLOB)";

const CREATE_SUB_SONGS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS sub_songs(
            url_id INTEGER,
            title STRING,
            length REAL,
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
    pub connection: sqlite::Connection,
}

impl SongDb {
    ///
    /// Open db and/or create the needed tables if they don't exist
    ///
    pub fn new(path: &str) -> Result<SongDb, Error> {
        let connection = sqlite::open(path)?;
        connection.execute(CREATE_URLS_TABLE)?;
        connection.execute(CREATE_SUB_SONGS_TABLE)?;
        connection.execute(CREATE_SAMPLES_TABLE)?;
        connection.execute(CREATE_INSTRUMENTS_TABLE)?;
        Ok(SongDb { connection })
    }

    ///
    /// Create an url and insert it into the urls table
    ///
    pub fn create_url(&self, url: &str) -> Result<u64, Error> {
        // TODO: Batch
        let url_id = XXH3_64::hash(url.as_bytes());
        let t = format!(
            "INSERT or IGNORE into urls (pk, path) VALUES({}, \"{}\")",
            url_id, url
        );
        self.connection.execute(t)?;
        Ok(url_id)
    }

    pub fn add_sub_song(&self, url_id: u64, name: &str, length: f64) -> Result<(), Error> {
        // TODO: Batch
        let t = format!(
            "INSERT or IGNORE into sub_songs (url_id, title, length) VALUES({}, \"{}\", {})",
            url_id, name, length
        );
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn add_sample(&self, url_id: u64, sample_text: &str) -> Result<(), Error> {
        // TODO: Batch
        let t = format!(
            "INSERT into samples (url_id, text) VALUES({}, \"{}\")",
            url_id, sample_text
        );
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn add_instrument(&self, url_id: u64, instrument_text: &str) -> Result<(), Error> {
        // TODO: Batch
        let t = format!(
            "INSERT into instruments (url_id, text) VALUES({}, \"{}\")",
            url_id, instrument_text
        );
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn set_tag_string(&self, url_id: u64, key: &str, data: &str) -> Result<(), Error> {
    	// TODO: Batch
        let t = format!("UPDATE urls SET {} = \"{}\" where pk == {}", key, data, url_id);
        self.connection.execute(t)?;
        Ok(())
    }

    pub fn set_tag_float(&self, url_id: u64, key: &str, data: f32) -> Result<(), Error> {
    	// TODO: Batch
        let t = format!("UPDATE urls SET {} = \"{}\" where pk == {}", key, data, url_id);
        self.connection.execute(t)?;
        Ok(())
    }
}

/// C interface exposed to plugins

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
        db.add_sub_song(url_id, "test title", 1.0).unwrap();
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
}
