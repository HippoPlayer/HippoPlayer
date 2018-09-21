
use plugin_handler;
use plugin_handler::Plugins;
use hippo_api::ffi::{CHippoPlaybackPlugin};

pub type DecoderPlugin = plugin_handler::Plugin<CHippoPlaybackPlugin>;

pub struct DecoderPlugins<'a> {
    pub handler: Plugins<'a, CHippoPlaybackPlugin>,
}

impl<'a> DecoderPlugins<'a> {
    pub fn new() -> DecoderPlugins<'a> {
        let mut dp = DecoderPlugins {
            handler: Plugins::<CHippoPlaybackPlugin>::new(),
        };

        dp.handler.add_plugins_from_path(b"hippo_playback_plugin");

        dp
    }
}
