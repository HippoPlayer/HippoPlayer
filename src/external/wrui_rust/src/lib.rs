extern crate libloading;

pub mod ffi_gen;
pub mod wrui;

use libloading::{Library, Symbol};
use std::rc::Rc;
use std::path::Path;

pub use wrui::Ui;

pub use wrui::*;

pub struct SharedLibUi {
    _lib: Rc<libloading::Library>,
    c_api: *const ffi_gen::PU,
}

// Path configurations for wrui
//
// macOS
//

#[cfg(target_os="macos")]
pub fn get_wrui_name() -> &'static str {
    "libwrui_qt.dylib"
}

#[cfg(all(target_os="macos", not(debug_assertions)))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/macosx-clang-release-default"
}

#[cfg(all(target_os="macos", debug_assertions))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/macosx-clang-debug-default"
}

//
// Linux
//

#[cfg(target_os="linux")]
pub fn get_wrui_name() -> &'static str {
    "libwrui_qt.so"
}

#[cfg(all(target_os="linux", not(debug_assertions)))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/linux-gcc-release-default"
}

#[cfg(all(target_os="linux", debug_assertions))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/linux-gcc-debug-default"
}

//
// Windows
//

#[cfg(target_os="windows")]
pub fn get_wrui_name() -> &'static str {
    "wrui_qt.dll"
}

#[cfg(all(target_os="windows", not(debug_assertions)))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/win64-msvc-release-default"
}

#[cfg(all(target_os="windows", debug_assertions))]
pub fn get_wrui_path() -> &'static str {
    "t2-output/win64-msvc-debug-default"
}


//
//
//

impl SharedLibUi {
    pub fn new() -> Option<SharedLibUi> {
        let lib;

        let wrui_filename = get_wrui_name();

        // If wrui is found it next to the executable we load that first. This only happens in a
        // release build but the check for an existing file is fast so we have it like this
        // Otherwise we load the file from the tundra output directory

        if Path::new(wrui_filename).is_file() {
            lib = Rc::new(Library::new(wrui_filename).unwrap());
            println!("loading {}", wrui_filename);
        } else {
            let path = Path::new(get_wrui_path()).join(wrui_filename);
            println!("loading {:?}", path);
            lib = Rc::new(Library::new(path).unwrap());
        }

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
