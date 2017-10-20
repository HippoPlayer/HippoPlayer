use dynamic_reload::{DynamicReload, Lib, Symbol, Search, PlatformName};
use std::os::raw::{c_int, c_void, c_char};
use std::sync::Arc;
use std::ffi::CString;

#[derive(Clone, Debug)]
pub struct CHippoPlaybackPlugin {
    pub version: u64,
    pub info: extern "C" fn(user_data: *mut c_void) -> *const c_char,
    pub track_info: extern "C" fn(user_data: *mut c_void) -> *const c_char,
    pub supported_extensions: extern "C" fn() -> *mut c_char,
    pub create: extern "C" fn() -> *mut c_void,
    pub destroy: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub open: extern "C" fn(user_data: *mut c_void, buffer: *const i8) -> c_int,
    pub close: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub read_data: extern "C" fn(user_data: *mut c_void, dest: *mut u8) -> c_int,
    pub seek: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub frame_size: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub private_data: u64,
}

#[derive(Clone)]
pub struct DecoderPlugin {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: CHippoPlaybackPlugin,
}

impl DecoderPlugin {
    pub fn is_ext_supported(&self, ext: &str) -> bool {
        let supported_ext; 
        unsafe {
            let temp = CString::from_raw(((self.plugin_funcs).supported_extensions)());
            supported_ext = temp.into_string().unwrap();
        }

        for e in supported_ext.split(",") {
            if ext == e {
                return true;
            }
        }

        false
    }
}

pub struct Plugins<'a> {
    pub decoder_plugins: Vec<DecoderPlugin>,
    pub plugin_handler: DynamicReload<'a>,
}

impl <'a> Plugins<'a> {
    pub fn new() -> Plugins<'a> {
        Plugins {
            decoder_plugins: Vec::new(),
            plugin_handler: DynamicReload::new(Some(vec!["t2-output/macosx-clang-debug-default"]), Some("t2-output"), Search::Default),
        }
    }

    fn add_dec_plugin(&mut self, plugin: &Arc<Lib>) {
        let fun: Symbol<extern "C" fn() -> *const CHippoPlaybackPlugin> = unsafe {
            plugin.lib.get(b"getPlugin\0").unwrap()
        };

        println!("Found plugin with callback data {:?}", fun());

        self.decoder_plugins.push(DecoderPlugin {
            plugin: plugin.clone(),
            plugin_path: "".to_owned(),
            plugin_funcs: unsafe { (*fun()).clone() },
        });
    }

    pub fn add_decoder_plugin(&mut self, name: &str) {
        match self.plugin_handler.add_library(name, PlatformName::Yes) {
            Ok(lib) => self.add_dec_plugin(&lib),
            Err(e) => {
                println!("Unable to load dynamic lib, err {:?}", e);
                return;
            }
        }
    }

}

