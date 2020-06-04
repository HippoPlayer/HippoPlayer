use config::Config;
use serde_derive::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Default)]
pub(crate) struct CoreConfig {
    /// Sorting for for playback plugins (lowest priority)
    pub(crate) playback_priority_low: Vec<String>,
    /// Sorting for for playback plugins (highest priority)
    pub(crate) playback_priority_high: Vec<String>,
    /// Auto switch track after x number of sec if there is no time for it
    pub(crate) playback_switch_no_time: f32,
}

impl CoreConfig {
    pub fn load(path: &str) -> Result<CoreConfig, String> {
        let t: CoreConfig = Config::deserialize(path)?;
        Ok(t)
    }
}
