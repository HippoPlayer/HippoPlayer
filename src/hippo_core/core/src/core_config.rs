use config::Config;
use std::path::PathBuf;
use serde_derive::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
pub struct CoreConfig {
    pub(crate) audio_device: String,
    /// Sorting for for playback plugins (lowest priority)
    pub(crate) playback_priority: Vec<String>,
    /// Auto switch track after x number of sec if there is no time for it
    pub(crate) playback_switch_no_time: f32,
}

impl Default for CoreConfig {
    fn default() -> CoreConfig {
        CoreConfig {
            audio_device: "Default Sound Device".to_owned(),
            playback_priority: vec!["openmpt".to_owned(), "uade".to_owned()],
            playback_switch_no_time: 5.0 * 60.0,
        }
    }
}

/// TODO: Cleanup recration of config filename
impl CoreConfig {
    pub fn load(path: &PathBuf, filename: &str) -> std::io::Result<CoreConfig> {
        let dir = path.join(filename);
        Config::load(&dir.to_string_lossy())
    }

    pub fn write(&self, path: &PathBuf, filename: &str) -> std::io::Result<usize> {
        let dir = path.join(filename);
        Config::write(&self, &dir.to_string_lossy())
    }
}
