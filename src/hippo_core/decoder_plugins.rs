use plugin_handler::Plugins;
use hippo_api::ffi::{CHippoPlaybackPlugin};

pub type DecoderPlugin = plugin_handler::Plugin<CHippoPlaybackPlugin>;

struct DecoderPlugins<'a> {
    plugins: Plugins<'a, CHippoPlaybackPlugin>,
}

impl DecoderPlugins {
    fn new() -> DecoderPlugins {
    let mut dp = DecoderPlugins {
        plugins: Plugins::<CHippoPlaybackPlugin>::new(),
    };

    dp.plugins.add_plugins_from_path(b"hippo_playback_plugin");

    dp
}
