use serde_derive::{Serialize, Deserialize};
use config::Config;

#[derive(Serialize, Deserialize, PartialEq, Eq, Default)]
pub(crate) struct CoreConfig {
	/// Sorting for for playback plugins (lowest priority)
    pub(crate) playback_priority_low: Vec<String>,
	/// Sorting for for playback plugins (highest priority)
    pub(crate) playback_priority_high: Vec<String>,
}

impl CoreConfig {
	pub fn load(path: &str) -> Result<CoreConfig, String> {
		let t: CoreConfig = Config::deserialize(path)?;
		Ok(t)
	}
}
