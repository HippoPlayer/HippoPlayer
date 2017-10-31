extern crate libloading;
mod ffi;

pub use ffi::WURect as Rect;
pub use ffi::WUPos as Pos;
pub use ffi::WUColor as Color;

use libloading::{Library, Symbol};
use std::rc::Rc;

impl Color {
    pub fn new(r: f32, g: f32, b: f32, a: f32) -> Color {
        Color { r: r, g: g, b: b, a: a }
    }
}

impl Pos {
    pub fn new(x: f32, y: f32) -> Pos {
        Pos { x: x, y: y }
    }
}

impl Rect {
    pub fn new(x: f32, y: f32, width: f32, height: f32) -> Rect {
        Rect { x: x, y: y, width: width, height: height }
    }
}

pub struct Wrui {
    _lib: Rc<libloading::Library>,
    c_api: *const ffi::Wrui,
    wu_app: *const ffi::WUApplication,
}

pub struct Window {
    pub wu_window: *const ffi::WUWindow,
    pub c_api: *const ffi::Wrui,
}

pub struct Painter {
    painter: *const ffi::WUPainter,
}

impl Painter {
    pub fn draw_rect(&self, rect: Rect, color: Color) {
        unsafe {
            (*self.painter).draw_rect.unwrap()(self.painter, rect, color);
        }
    }

    pub fn draw_text(&self, pos: Pos, color: Color, text: &str) {
        unsafe {
            (*self.painter).draw_text.unwrap()(self.painter, pos, color, text.as_ptr() as *const i8, text.len() as i32, std::ptr::null());
        }
    }
}

#[macro_export]
macro_rules! window_set_paint_event {
    ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
        {
            extern "C" fn temp_paint_event(target: *mut std::os::raw::c_void) {
                unsafe {
                    let app = target as *mut $call_type;
                    $callback(&mut *app);
                }
            }

            unsafe {
                let window_funcs = *(*$sender.c_api).window_funcs;
                window_funcs.set_paint_event.unwrap()($sender.wu_window, std::mem::transmute($data), temp_paint_event);
            }
        }
    }
}

impl Wrui {
    pub fn new(shared_lib: &str) -> Option<Wrui> {
        let lib = Rc::new(Library::new(shared_lib).unwrap());
        unsafe {
            let wrui_get: Symbol<unsafe extern fn() -> *const ffi::Wrui> =
                lib.get(b"wrui_get\0").unwrap();

            let c_api = wrui_get();

            Some(Wrui {
                _lib: lib.clone(),
                c_api: wrui_get(),
                wu_app: (*c_api).application_create.unwrap()(),
            })
        }
    }

    pub fn get_painter(&self) -> Painter {
        Painter {
            painter: unsafe { (*self.c_api).painter_get.unwrap()() },
        }
    }

    pub fn create_window(&self) -> Window {
        let wu_window = unsafe {
            (*self.c_api).window_create.unwrap()(std::ptr::null_mut())
        };

        Window {
            wu_window: wu_window,
            c_api: self.c_api,
        }
    }

    pub fn run(&self) {
        unsafe {
            (*(*self.c_api).application_funcs).run.unwrap()(self.wu_app);
        }
    }
}

