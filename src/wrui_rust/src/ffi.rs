#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct WUWindow {
    _unused: [u8; 0],
}
pub type WUInternalHandle = *mut ::std::os::raw::c_void;
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct Wrui {
    pub api_version: u64,
    pub application_create: ::std::option::Option<unsafe extern "C" fn() -> *const WUApplication>,
    pub window_create: ::std::option::Option<unsafe extern "C" fn(parent: WUInternalHandle)
                                                                    -> *mut WUWindow>,
    pub painter_get: ::std::option::Option<unsafe extern "C" fn() -> *mut WUPainter>,
    pub window_funcs: *const WUWindowFuncs,
    pub application_funcs: *const WUApplicationFuncs,
}
#[test]
fn bindgen_test_layout_Wrui() {
    assert_eq!(::std::mem::size_of::<Wrui>(),
               48usize,
               concat!("Size of: ", stringify!(Wrui)));
    assert_eq!(::std::mem::align_of::<Wrui>(),
               8usize,
               concat!("Alignment of ", stringify!(Wrui)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).api_version as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(api_version)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).application_create as *const _ as usize },
               8usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(application_create)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).window_create as *const _ as usize },
               16usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(window_create)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).painter_get as *const _ as usize },
               24usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(painter_get)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).window_funcs as *const _ as usize },
               32usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(window_funcs)));
    assert_eq!(unsafe { &(*(0 as *const Wrui)).application_funcs as *const _ as usize },
               40usize,
               concat!("Alignment of field: ",
                       stringify!(Wrui),
                       "::",
                       stringify!(application_funcs)));
}
impl Clone for Wrui {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUApplication {
    pub handle: WUInternalHandle,
}
#[test]
fn bindgen_test_layout_WUApplication() {
    assert_eq!(::std::mem::size_of::<WUApplication>(),
               8usize,
               concat!("Size of: ", stringify!(WUApplication)));
    assert_eq!(::std::mem::align_of::<WUApplication>(),
               8usize,
               concat!("Alignment of ", stringify!(WUApplication)));
    assert_eq!(unsafe { &(*(0 as *const WUApplication)).handle as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUApplication),
                       "::",
                       stringify!(handle)));
}
impl Clone for WUApplication {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUApplicationFuncs {
    pub run: ::std::option::Option<unsafe extern "C" fn(p: *const WUApplication)
                                                          -> ::std::os::raw::c_int>,
}
#[test]
fn bindgen_test_layout_WUApplicationFuncs() {
    assert_eq!(::std::mem::size_of::<WUApplicationFuncs>(),
               8usize,
               concat!("Size of: ", stringify!(WUApplicationFuncs)));
    assert_eq!(::std::mem::align_of::<WUApplicationFuncs>(),
               8usize,
               concat!("Alignment of ", stringify!(WUApplicationFuncs)));
    assert_eq!(unsafe { &(*(0 as *const WUApplicationFuncs)).run as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUApplicationFuncs),
                       "::",
                       stringify!(run)));
}
impl Clone for WUApplicationFuncs {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUPos {
    pub x: f32,
    pub y: f32,
}
#[test]
fn bindgen_test_layout_WUPos() {
    assert_eq!(::std::mem::size_of::<WUPos>(),
               8usize,
               concat!("Size of: ", stringify!(WUPos)));
    assert_eq!(::std::mem::align_of::<WUPos>(),
               4usize,
               concat!("Alignment of ", stringify!(WUPos)));
    assert_eq!(unsafe { &(*(0 as *const WUPos)).x as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUPos),
                       "::",
                       stringify!(x)));
    assert_eq!(unsafe { &(*(0 as *const WUPos)).y as *const _ as usize },
               4usize,
               concat!("Alignment of field: ",
                       stringify!(WUPos),
                       "::",
                       stringify!(y)));
}
impl Clone for WUPos {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WURect {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
}
#[test]
fn bindgen_test_layout_WURect() {
    assert_eq!(::std::mem::size_of::<WURect>(),
               16usize,
               concat!("Size of: ", stringify!(WURect)));
    assert_eq!(::std::mem::align_of::<WURect>(),
               4usize,
               concat!("Alignment of ", stringify!(WURect)));
    assert_eq!(unsafe { &(*(0 as *const WURect)).x as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WURect),
                       "::",
                       stringify!(x)));
    assert_eq!(unsafe { &(*(0 as *const WURect)).y as *const _ as usize },
               4usize,
               concat!("Alignment of field: ",
                       stringify!(WURect),
                       "::",
                       stringify!(y)));
    assert_eq!(unsafe { &(*(0 as *const WURect)).width as *const _ as usize },
               8usize,
               concat!("Alignment of field: ",
                       stringify!(WURect),
                       "::",
                       stringify!(width)));
    assert_eq!(unsafe { &(*(0 as *const WURect)).height as *const _ as usize },
               12usize,
               concat!("Alignment of field: ",
                       stringify!(WURect),
                       "::",
                       stringify!(height)));
}
impl Clone for WURect {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUColor {
    pub r: f32,
    pub g: f32,
    pub b: f32,
    pub a: f32,
}
#[test]
fn bindgen_test_layout_WUColor() {
    assert_eq!(::std::mem::size_of::<WUColor>(),
               16usize,
               concat!("Size of: ", stringify!(WUColor)));
    assert_eq!(::std::mem::align_of::<WUColor>(),
               4usize,
               concat!("Alignment of ", stringify!(WUColor)));
    assert_eq!(unsafe { &(*(0 as *const WUColor)).r as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUColor),
                       "::",
                       stringify!(r)));
    assert_eq!(unsafe { &(*(0 as *const WUColor)).g as *const _ as usize },
               4usize,
               concat!("Alignment of field: ",
                       stringify!(WUColor),
                       "::",
                       stringify!(g)));
    assert_eq!(unsafe { &(*(0 as *const WUColor)).b as *const _ as usize },
               8usize,
               concat!("Alignment of field: ",
                       stringify!(WUColor),
                       "::",
                       stringify!(b)));
    assert_eq!(unsafe { &(*(0 as *const WUColor)).a as *const _ as usize },
               12usize,
               concat!("Alignment of field: ",
                       stringify!(WUColor),
                       "::",
                       stringify!(a)));
}
impl Clone for WUColor {
    fn clone(&self) -> Self {
        *self
    }
}
///
pub type PaintEvent = unsafe extern "C" fn(user_data: *mut ::std::os::raw::c_void);
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUFont {
    pub set_size: ::std::option::Option<unsafe extern "C" fn(font:
                                                                 *mut WUFont,
                                                             size:
                                                                 ::std::os::raw::c_int)
                                            -> ::std::os::raw::c_int>,
    pub set_font_from_memory: ::std::option::Option<unsafe extern "C" fn(font:
                                                                             *mut WUFont,
                                                                         data:
                                                                             *mut ::std::os::raw::c_void,
                                                                         size:
                                                                             ::std::os::raw::c_int)
                                                        ->
                                                            ::std::os::raw::c_int>,
    pub set_font_from_filename: ::std::option::Option<unsafe extern "C" fn(font:
                                                                               *mut WUFont,
                                                                           filename:
                                                                               *const ::std::os::raw::c_char,
                                                                           len:
                                                                               ::std::os::raw::c_int)
                                                          ->
                                                              ::std::os::raw::c_int>,
}
#[test]
fn bindgen_test_layout_WUFont() {
    assert_eq!(::std::mem::size_of::<WUFont>(),
               24usize,
               concat!("Size of: ", stringify!(WUFont)));
    assert_eq!(::std::mem::align_of::<WUFont>(),
               8usize,
               concat!("Alignment of ", stringify!(WUFont)));
    assert_eq!(unsafe { &(*(0 as *const WUFont)).set_size as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUFont),
                       "::",
                       stringify!(set_size)));
    assert_eq!(unsafe { &(*(0 as *const WUFont)).set_font_from_memory as *const _ as usize },
               8usize,
               concat!("Alignment of field: ",
                       stringify!(WUFont),
                       "::",
                       stringify!(set_font_from_memory)));
    assert_eq!(unsafe { &(*(0 as *const WUFont)).set_font_from_filename as *const _ as usize },
               16usize,
               concat!("Alignment of field: ",
                       stringify!(WUFont),
                       "::",
                       stringify!(set_font_from_filename)));
}
impl Clone for WUFont {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUPainter {
    pub create_font: ::std::option::Option<unsafe extern "C" fn()
                                               -> *mut WUFont>,
    pub draw_text: ::std::option::Option<unsafe extern "C" fn(painter:
                                                                  *const WUPainter,
                                                              pos: WUPos,
                                                              color: WUColor,
                                                              text:
                                                                  *const ::std::os::raw::c_char,
                                                              len:
                                                                  ::std::os::raw::c_int,
                                                              font:
                                                                  *const WUFont)>,
    pub draw_rect: ::std::option::Option<unsafe extern "C" fn(painter:
                                                                  *const WUPainter,
                                                              rect: WURect,
                                                              color:
                                                                  WUColor)>,
}
#[test]
fn bindgen_test_layout_WUPainter() {
    assert_eq!(::std::mem::size_of::<WUPainter>(),
               24usize,
               concat!("Size of: ", stringify!(WUPainter)));
    assert_eq!(::std::mem::align_of::<WUPainter>(),
               8usize,
               concat!("Alignment of ", stringify!(WUPainter)));
    assert_eq!(unsafe { &(*(0 as *const WUPainter)).create_font as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUPainter),
                       "::",
                       stringify!(create_font)));
    assert_eq!(unsafe { &(*(0 as *const WUPainter)).draw_text as *const _ as usize },
               8usize,
               concat!("Alignment of field: ",
                       stringify!(WUPainter),
                       "::",
                       stringify!(draw_text)));
    assert_eq!(unsafe { &(*(0 as *const WUPainter)).draw_rect as *const _ as usize },
               16usize,
               concat!("Alignment of field: ",
                       stringify!(WUPainter),
                       "::",
                       stringify!(draw_rect)));
}
impl Clone for WUPainter {
    fn clone(&self) -> Self {
        *self
    }
}
///
#[repr(C)]
#[derive(Debug, Copy)]
pub struct WUWindowFuncs {
    pub set_paint_event: ::std::option::Option<unsafe extern "C" fn(window:
                                                                        *const WUWindow,
                                                                    user_data:
                                                                        *const ::std::os::raw::c_void,
                                                                    event:
                                                                        PaintEvent)>,
}
#[test]
fn bindgen_test_layout_WUWindowFuncs() {
    assert_eq!(::std::mem::size_of::<WUWindowFuncs>(),
               8usize,
               concat!("Size of: ", stringify!(WUWindowFuncs)));
    assert_eq!(::std::mem::align_of::<WUWindowFuncs>(),
               8usize,
               concat!("Alignment of ", stringify!(WUWindowFuncs)));
    assert_eq!(unsafe { &(*(0 as *const WUWindowFuncs)).set_paint_event as *const _ as usize },
               0usize,
               concat!("Alignment of field: ",
                       stringify!(WUWindowFuncs),
                       "::",
                       stringify!(set_paint_event)));
}
impl Clone for WUWindowFuncs {
    fn clone(&self) -> Self {
        *self
    }
}
extern "C" {
// pub fn wrui_get() -> *mut Wrui;
}
