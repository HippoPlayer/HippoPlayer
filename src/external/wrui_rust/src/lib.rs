extern crate libloading;

pub mod ffi_gen;
pub mod wrui;

use libloading::{Library, Symbol};
use std::rc::Rc;

pub use wrui::Ui;

pub use wrui::*;

pub struct SharedLibUi {
    _lib: Rc<libloading::Library>,
    c_api: *const ffi_gen::PU,
}

#[cfg(target_os="macos")]
pub fn get_wrui_name() -> &'static str {
    "libwrui_qt.dylib"
}

#[cfg(target_os="linux")]
pub fn get_wrui_name() -> &'static str {
    "libwrui_qt.so"
}

#[cfg(target_os="windows")]
pub fn get_wrui_name() -> &'static str {
    "wrui_qt.dll"
}

impl SharedLibUi {
    pub fn new() -> Option<SharedLibUi> {
        let wrui_filename = get_wrui_name();

        // TODO: We should do better error handling here
        // This to enforce we load relative to the current exe

        let current_exe = std::env::current_exe().unwrap();

        std::env::set_current_dir(current_exe.parent().unwrap()).unwrap();

        let path = std::env::current_dir().unwrap();

        println!("The current directory is {}", path.display());

        // If wrui is found it next to the executable we load that first. This only happens in a
        // release build but the check for an existing file is fast so we have it like this
        // Otherwise we load the file from the tundra output directory

        let lib = Rc::new(Library::new(wrui_filename).unwrap());

        unsafe {
            let wrui_get: Symbol<unsafe extern "C" fn() -> *const ffi_gen::PU> =
                lib.get(b"wrui_get\0").unwrap();

            Some(SharedLibUi {
                _lib: lib.clone(),
                c_api: wrui_get(),
            })
        }
    }

    pub fn get_ui(&self) -> Ui {
        Ui::new(self.c_api)
    }
}
