extern crate libloading;

pub mod ffi_gen;
pub mod wrui;

use libloading::{Library, Symbol};
use std::rc::Rc;

pub use wrui::Ui;

pub struct SharedLibUi {
    _lib: Rc<libloading::Library>,
    c_api: *const ffi_gen::PU,
}

impl SharedLibUi {
    pub fn new(shared_lib: &str) -> Option<SharedLibUi> {
        let lib = Rc::new(Library::new(shared_lib).unwrap());
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
