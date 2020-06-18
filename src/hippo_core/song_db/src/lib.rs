use sqlite;
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
            length STRING,
            ripper STRING,
            extra_data BLOB)";

const CREATE_SUB_SONGS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS sub_songs(
            FOREGIN KEY(sub_song_url_id) REFERENCES urls(url_id),
            tile STRING,
            length STRING,
            extra_data BLOB)";

const CREATE_SAMPLES_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS samples(
            FOREGIN KEY(sample_url_id) REFERENCES urls(url_id),
            sample STRING)";

const CREATE_INSTRUMENTS_TABLE: &str = "
        CREATE TABLE IF NOT EXISTS samples(
            FOREGIN KEY(instrument_url_id) REFERENCES urls(url_id),
            instrument STRING)";

struct SongDb {
    connection: sqlite::Connection,
}

impl SongDb {
    ///
    /// Open db and/or create the needed tables if they don't exist
    ///
    pub fn new(path: &str) -> Result<SongDb, sqlite::Error> {
        let connection = sqlite::open(path)?;
        connection.execute(URL_CREATE_TABLE)?;
        connection.execute(URL_INSTRUMENTS_TABLE)?;
        connection.execute(URL_SAMPLES_TABLE)?;
        connection.execute(URL_INSTRUMENTS_TABLE)?;
        Ok(SongDb { connection })
    }

    pub fn create_url(&self, url: &str) -> Result<u64, sqlite::Error> {
        // TODO: Batch
        let url_id = XXH3_64::hash(url.as_bytes());
        let t = format!("INSERT into urls or IGNORE (pk, path) VALUES({}, \"{}\")", url_id, url);
        self.connection.execute(t)?;
        Ok(url_id)
    }
}

/// C interface exposed to plugins


#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
