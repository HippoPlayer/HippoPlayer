use config::Config;
use std::path::PathBuf;
use serde_derive::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Default)]
pub struct CoreConfig {
    pub(crate) audio_device: Option<String>,
    /// Sorting for for playback plugins (lowest priority)
    pub(crate) playback_priority: Vec<String>,
    /// Auto switch track after x number of sec if there is no time for it
    pub(crate) playback_switch_no_time: f32,
}

impl CoreConfig {
    pub fn load(path: &PathBuf, filename: &str) -> std::io::Result<CoreConfig> {
        let dir = path.join(filename);
        Config::load(&dir.to_string_lossy())
    }

    pub fn write(&self, path: &str) -> std::io::Result<usize> {
        Config::write(&self, path)
    }
}
