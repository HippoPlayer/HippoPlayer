use serde::de::DeserializeOwned;
use serde::Serialize;
use std::fs::File;
use std::io;
use std::io::Read;
use std::io::{Error, ErrorKind, Write};
use toml;

pub struct Config;

impl Config {
    fn read_to_file(path: &str) -> io::Result<String> {
        let mut data = String::new();
        let mut file = File::open(&path)?;
        file.read_to_string(&mut data)?;
        Ok(data)
    }

    pub fn deserialize<'a, T: DeserializeOwned>(path: &str) -> io::Result<T> {
        let data = Self::read_to_file(path)?;

        let t: T = match toml::from_str(&data) {
            Err(e) => {
                return Err(Error::new(
                    ErrorKind::Other,
                    format!("Unable to parse config file {}: {}", path, e),
                ))
            }
            Ok(v) => v,
        };

        Ok(t)
    }

    pub fn serialize<T: Serialize>(config: &T, path: &str) -> io::Result<usize> {
        let mut file = File::create(&path)?;
        let toml = match toml::to_string(config) {
            Err(e) => {
                return Err(Error::new(
                    ErrorKind::Other,
                    format!("Unable to write config file {}: {}", path, e),
                ))
            }
            Ok(v) => v,
        };

        file.write(toml.as_bytes())
    }
}
