use dynamic_reload::{DynamicReload, Lib, PlatformName, Search, Symbol};
use std::ffi::CString;
use std::sync::Arc;
use walkdir::{DirEntry, WalkDir};
use ffi;

//use hippo_api::ffi::{CHippoPlaybackPlugin};
//use crate::service_ffi::{PluginService};

#[derive(Clone)]
pub struct DecoderPlugin {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: ffi::HippoPlaybackPlugin,
}

#[cfg(target_os = "macos")]
pub fn get_plugin_ext() -> &'static str {
    "dylib"
}

#[cfg(target_os = "linux")]
pub fn get_plugin_ext() -> &'static str {
    "so"
}

#[cfg(target_os = "windows")]
pub fn get_plugin_ext() -> &'static str {
    "dll"
}

impl DecoderPlugin {
    pub fn probe_can_play(
        &self,
        data: &[u8],
        buffer_len: usize,
        filename: &str,
        file_size: u64,
    ) -> bool {
        let c_filename = CString::new(filename).unwrap();
        let res = ((self.plugin_funcs).probe_can_play)(
            data.as_ptr(),
            buffer_len as u32,
            c_filename.as_ptr(),
            file_size,
        );

        match res {
            0 => true,
            _ => false,
        }
    }
}

pub struct Plugins {
    pub decoder_plugins: Vec<DecoderPlugin>,
    pub plugin_handler: DynamicReload,
}

impl Plugins {
    pub fn new() -> Plugins{
        Plugins {
            decoder_plugins: Vec::new(),
            plugin_handler: DynamicReload::new(Some(vec!["."]), None, Search::Default),
        }
    }

    fn add_plugin_lib(&mut self, name: &str, plugin: &Arc<Lib>) {
        let func: Result<Symbol<extern "C" fn() -> *const ffi::HippoPlaybackPlugin>, ::std::io::Error> =
            unsafe { plugin.lib.get(b"hippo_playback_plugin\0") };

        if let Ok(fun) = func {
            println!("Found playback plugin with callback data {:?}", fun());

            self.decoder_plugins.push(DecoderPlugin {
                plugin: plugin.clone(),
                plugin_path: name.to_owned(),
                plugin_funcs: unsafe { (*fun()).clone() },
            });
        }
    }

    fn check_file_type(entry: &DirEntry) -> bool {
        let path = entry.path();

        if let Some(ext) = path.extension() {
            ext == get_plugin_ext()
        } else {
            false
        }
    }

    fn internal_add_plugins_from_path(&mut self, path: &str) {
        for entry in WalkDir::new(path).max_depth(1) {
            if let Ok(t) = entry {
                if Self::check_file_type(&t) {
                    self.add_plugin(t.path().to_str().unwrap());
                    //println!("{}", t.path().display());
                }
            }
        }
    }

    pub fn add_plugins_from_path(&mut self) {
        self.internal_add_plugins_from_path("plugins");
        self.internal_add_plugins_from_path(".");
    }

    pub fn add_plugin(&mut self, name: &str) {
        match self.plugin_handler.add_library(name, PlatformName::No) {
            Ok(lib) => self.add_plugin_lib(name, &lib),
            Err(e) => {
                println!("Unable to load dynamic lib, err {:?}", e);
                return;
            }
        }
    }
}
