use dynamic_reload::{DynamicReload, Lib, Symbol, Search, PlatformName};
use std::sync::Arc;
use walkdir::{WalkDir, DirEntry};

#[derive(Clone)]
pub struct Plugin<T> {
    pub plugin: Arc<Lib>,
    pub plugin_path: String,
    pub plugin_funcs: T,
}

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

pub struct Plugins<'a, T: Clone> {
    pub plugins: Vec<Plugin<T>>,
    pub plugin_handler: DynamicReload<'a>,
}

impl <'a, T: Clone> Plugins<'a, T> {
    pub fn new() -> Plugins<'a, T>
        where T: Clone,
    {
        Plugins {
            plugins: Vec::new(),
            plugin_handler: DynamicReload::new(Some(vec!["."]), None, Search::Default),
        }
    }

    fn add_plugin_lib(&mut self, name: &str, plugin: &Arc<Lib>, entry_name: &[u8]) {
        let func: Result<Symbol<extern "C" fn() -> *const T>, ::std::io::Error> = unsafe {
            plugin.lib.get(entry_name)
        };

        if let Ok(fun) = func {
            self.plugins.push(Plugin {
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

    fn internal_add_plugins_from_path(&mut self, path: &str, entry_name: &[u8]) {
        for entry in WalkDir::new(path).max_depth(1) {
            if let Ok(t) = entry {
                if Self::check_file_type(&t) {
                    self.add_plugin(t.path().to_str().unwrap(), entry_name);
                }
            }
        }
    }

    pub fn add_plugins_from_path(&mut self, entry_name: &[u8]) {
        self.internal_add_plugins_from_path("plugins", entry_name);
        self.internal_add_plugins_from_path(".", entry_name);
    }

    pub fn add_plugin(&mut self, name: &str, entry_name: &[u8]) {
        match self.plugin_handler.add_library(name, PlatformName::No) {
            Ok(lib) => self.add_plugin_lib(name, &lib, entry_name),
            Err(e) => {
                println!("Unable to load dynamic lib, err {:?}", e);
                return;
            }
        }
    }

}

