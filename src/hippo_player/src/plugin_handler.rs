use dynamic_reload::{DynamicReload, Lib, Symbol, Search, PlatformName};
use std::os::raw::{c_uchar, c_int, c_void, c_uint, c_char};
use std::rc::Rc;

pub struct CHippoPlaybackBuffer {
    pub data: *mut c_uchar,
    pub read_offset: c_uint,
    pub frame_size: c_uint,
    pub frame_max_size: c_uint,
}

#[derive(Clone, Debug)]
pub struct CHippoPlaybackPlugin {
    pub version: c_int,
    pub info: extern "C" fn(user_data: *mut c_void) -> *const c_char,
    pub track_info: extern "C" fn(user_data: *mut c_void) -> *const c_char,
    pub supported_extensions: extern "C" fn(user_data: *mut c_void) -> *const *const c_char,
    pub create: extern "C" fn() -> *mut c_void,
    pub destroy: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub open: extern "C" fn(user_data: *mut c_void, buffer: *const c_char) -> c_int,
    pub close: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub read_data: extern "C" fn(user_data: *mut c_void, dest: *mut CHippoPlaybackBuffer) -> c_int,
    pub seek: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub frame_size: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub private_data: *mut c_void,
}

pub struct DecoderPlugin {
    pub plugin: Rc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: CHippoPlaybackPlugin,
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

    fn add_dec_plugin(plugin: &Rc<Lib>) {
        let fun: Symbol<extern "C" fn() -> *const CHippoPlaybackPlugin> = unsafe {
            plugin.lib.get(b"getPlugin\0").unwrap()
        };

        println!("Found plugin with callback data {:?}", fun());
    }

    pub fn add_decoder_plugin(&mut self, name: &str) {
        match self.plugin_handler.add_library(name, PlatformName::Yes) {
            Ok(lib) => Self::add_dec_plugin(&lib),
            Err(e) => {
                println!("Unable to load dynamic lib, err {:?}", e);
                return;
            }
        }
    }
}

