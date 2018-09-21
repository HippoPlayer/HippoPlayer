use dynamic_reload::{DynamicReload, Lib, Symbol, Search, PlatformName};
use walkdir::{WalkDir, DirEntry};
//use std::os::raw::{c_int, c_void, c_char};
use std::sync::Arc;
use std::ffi::{CString, CStr};
use std::borrow::Cow;

use hippo_api::ffi::{CHippoPlaybackPlugin, CHippoViewPlugin};
use service_ffi::{PluginService};
//use rute::rute::*;

#[derive(Clone)]
pub struct DecoderPlugin {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: CHippoPlaybackPlugin,
}

#[derive(Clone)]
pub struct ViewPlugin {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub count: usize,
    pub plugin_funcs: CHippoViewPlugin,
}

/*
pub struct ViewPluginInstance {
	pub plugin: ViewPlugin,
	pub user_data: u64,
	pub id: usize,
	pub ui: PluginUi,
}
*/

#[cfg(target_os="macos")]
pub fn get_plugin_ext() -> &'static str {
    "dylib"
}

#[cfg(target_os="linux")]
pub fn get_plugin_ext() -> &'static str {
    "so"
}

#[cfg(target_os="windows")]
pub fn get_plugin_ext() -> &'static str {
    "dll"
}

impl DecoderPlugin {
    pub fn probe_can_play(&self, data: &[u8], buffer_len: usize, filename: &str, file_size: u64) -> bool {
        let c_filename = CString::new(filename).unwrap();
        let res = ((self.plugin_funcs).probe_can_play)(data.as_ptr(), buffer_len as u32, c_filename.as_ptr(), file_size);

        match res {
            0 => true,
            _ => false,
        }
    }
}

/*
impl ViewPlugin {
	pub fn get_name<'a>(&'a self) -> Cow<'a, str> {
        //let res = unsafe { self.plugin_funcs.name };
        unsafe { CStr::from_ptr(self.plugin_funcs.name as *const i8).to_string_lossy() }
	}

	pub fn create_instance(&self, ui: &Ui, plugin_service: &PluginService, window: &Widget) -> ViewPluginInstance {
        let plugin_ui = ui.create_plugin_ui(window);

		let user_data = ((self.plugin_funcs).create).unwrap()(
			plugin_service.get_c_service_api());

		((self.plugin_funcs).setup_ui).unwrap()(user_data, plugin_ui.get_c_api());

		ViewPluginInstance {
			plugin: self.clone(),
			user_data: user_data as u64,
			id: self.count,
			ui: plugin_ui,
		}
	}
}
*/

pub struct Plugins<'a> {
    pub decoder_plugins: Vec<DecoderPlugin>,
    //pub view_plugins: Vec<ViewPlugin>,
    pub plugin_handler: DynamicReload<'a>,
}

impl <'a> Plugins<'a> {
    pub fn new() -> Plugins<'a> {
        Plugins {
            decoder_plugins: Vec::new(),
            //view_plugins: Vec::new(),
            plugin_handler: DynamicReload::new(Some(vec!["."]), None, Search::Default),
        }
    }

    fn add_plugin_lib(&mut self, name: &str, plugin: &Arc<Lib>) {
        let func: Result<Symbol<extern "C" fn() -> *const CHippoPlaybackPlugin>, ::std::io::Error> = unsafe {
            plugin.lib.get(b"hippo_playback_plugin\0")
        };

        if let Ok(fun) = func {
            println!("Found playback plugin with callback data {:?}", fun());

            self.decoder_plugins.push(DecoderPlugin {
                plugin: plugin.clone(),
                plugin_path: name.to_owned(),
                plugin_funcs: unsafe { (*fun()).clone() },
            });
        }

        /*
        let func: Result<Symbol<extern "C" fn() -> *const CHippoViewPlugin>, ::std::io::Error> = unsafe {
            plugin.lib.get(b"hippo_view_plugin\0")
        };

        if let Ok(fun) = func {
            println!("Found view plugin with callback data {:?}", fun());

            self.view_plugins.push(ViewPlugin {
                plugin: plugin.clone(),
                plugin_path: name.to_owned(),
                count: 0,
                plugin_funcs: unsafe { (*fun()).clone() },
            });
        }
        */
    }

    fn check_file_type(_entry: &DirEntry) -> bool {
        true
        /*
        let path = entry.path();

        if path.ends_with(::rute::get_rute_name()) {
            false
        } else if let Some(ext) = path.extension() {
            ext == get_plugin_ext()
        } else {
            false
        }
        */
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

