use crate::ffi;
use dynamic_reload::{DynamicReload, Lib, PlatformName, Search, Symbol};
use std::ffi::CStr;
use std::ffi::CString;
use std::os::raw::{c_char, c_void};
use std::sync::Arc;
use walkdir::{DirEntry, WalkDir};

//use hippo_api::ffi::{CHippoPlaybackPlugin};
use crate::service_ffi::PluginService;

#[derive(Debug, Clone)]
pub struct HippoPlaybackPluginFFI {
    pub api_version: u64,
    pub user_data: u64, // this is really a pointer but Rust gets sad when we use this on another thread so we hack it here a bit.
    pub name: String,
    pub version: String,
    pub probe_can_play: unsafe extern "C" fn(
        data: *const u8,
        data_size: u32,
        filename: *const c_char,
        total_size: u64,
    ) -> u32,
    pub supported_extensions: unsafe extern "C" fn() -> *const c_char,
    pub create: unsafe extern "C" fn(services: *const ffi::HippoServiceAPI) -> *mut c_void,
    pub destroy: unsafe extern "C" fn(user_data: *mut c_void) -> i32,
    pub event: Option<unsafe extern "C" fn(user_data: *mut c_void, data: *const u8, len: i32)>,

    pub open: unsafe extern "C" fn(user_data: *mut c_void, buffer: *const c_char, subsong: i32) -> i32,
    pub close: unsafe extern "C" fn(user_data: *mut c_void) -> i32,
    pub read_data: unsafe extern "C" fn(
        user_data: *mut c_void,
        dest: *mut c_void,
        max_sample_count: u32,
    ) -> i32,
    pub seek: unsafe extern "C" fn(user_data: *mut c_void, ms: i32) -> i32,
    pub metadata: Option<
        unsafe extern "C" fn(buffer: *const i8, services: *const ffi::HippoServiceAPI) -> i32,
    >,
    pub save: Option<
        unsafe extern "C" fn(user_data: *mut c_void, save_api: *const ffi::HippoSaveAPI) -> i32,
    >,
    pub load: Option<
        unsafe extern "C" fn(user_data: *mut c_void, load_api: *const ffi::HippoLoadAPI) -> i32,
    >,
}

#[derive(Clone)]
pub struct DecoderPlugin {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: HippoPlaybackPluginFFI,
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
        let res = unsafe {
            ((self.plugin_funcs).probe_can_play)(
                data.as_ptr(),
                buffer_len as u32,
                c_filename.as_ptr(),
                file_size,
            )
        };

        match res {
            0 => true,
            _ => false,
        }
    }

    pub fn get_metadata(&self, filename: &str, service: &PluginService) {
        let c_filename = CString::new(filename).unwrap();
        unsafe {
            if let Some(func) = self.plugin_funcs.metadata {
                let _ = func(c_filename.as_ptr(), service.get_c_service_api());
            }
        };
    }
}

pub struct Plugins {
    pub decoder_plugins: Vec<Box<DecoderPlugin>>,
    pub plugin_handler: DynamicReload,
}

impl Plugins {
    pub fn new() -> Plugins {
        Plugins {
            decoder_plugins: Vec::new(),
            plugin_handler: DynamicReload::new(Some(vec!["."]), None, Search::Default),
        }
    }

    fn add_plugin_lib(&mut self, name: &str, plugin: &Arc<Lib>) {
        let func: Result<
            Symbol<extern "C" fn() -> *const ffi::HippoPlaybackPlugin>,
            ::std::io::Error,
        > = unsafe { plugin.lib.get(b"hippo_playback_plugin\0") };

        if let Ok(fun) = func {
            println!("Found playback plugin with callback data {:?}", fun());

            let native_plugin = unsafe { *fun() };

            // To make the plugin code a bit nicer we move over to a separate structure internally.
            // This also allows us allows us to check functions are correct at one place instead of
            // having unwraps of function ptrs for every call
            let plugin_funcs = HippoPlaybackPluginFFI {
                api_version: native_plugin.api_version,
                user_data: 0,
                name: unsafe {
                    CStr::from_ptr(native_plugin.name)
                        .to_string_lossy()
                        .into_owned()
                },
                version: unsafe {
                    CStr::from_ptr(native_plugin.version)
                        .to_string_lossy()
                        .into_owned()
                },
                probe_can_play: native_plugin.probe_can_play.unwrap(),
                supported_extensions: native_plugin.supported_extensions.unwrap(),
                create: native_plugin.create.unwrap(),
                event: native_plugin.event,
                destroy: native_plugin.destroy.unwrap(),
                open: native_plugin.open.unwrap(),
                close: native_plugin.close.unwrap(),
                read_data: native_plugin.read_data.unwrap(),
                seek: native_plugin.seek.unwrap(),
                metadata: native_plugin.metadata,
                save: native_plugin.save,
                load: native_plugin.load,
            };

            self.decoder_plugins.push(Box::new(DecoderPlugin {
                plugin: plugin.clone(),
                plugin_path: name.to_owned(),
                plugin_funcs,
            }));
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
            }
        }
    }
}
