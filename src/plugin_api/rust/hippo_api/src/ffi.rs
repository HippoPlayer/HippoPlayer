use std::os::raw::{c_int, c_void, c_char};
use wrui::ffi_gen::PUPluginUI;

#[repr(C)]
#[derive(Debug)]
pub struct CHippoIoAPI {
    pub exists: extern "C" fn(priv_data: *const c_void, target: *const u8) -> c_int,
    pub read_file_to_memory: extern "C" fn(priv_data: *const c_void, filename: *const u8, target: *mut *const c_void, target_size: *mut u64) -> i64,
    pub free_file_to_memory: extern "C" fn(priv_data: *const c_void, handle: *const c_void) -> i64,
    pub open: extern "C" fn(priv_data: *const c_void, target: *const u8, handle: *mut *const c_void) -> i64,
    pub close: extern "C" fn(handle: *const c_void) -> i64,
    pub size: extern "C" fn(handle: *const c_void, res: *mut u64) -> i64,
    pub read: extern "C" fn(handle: *const c_void, target: *mut u8, size: u64) -> i64,
    pub seek: extern "C" fn(handle: *const c_void, seek_type: i32, seek_step: u64) -> i64,
    pub priv_data: *const c_void,
}

#[repr(C)]
#[derive(Debug)]
pub struct CMetadataAPI {
    pub get_key: extern "C" fn(priv_data: *const c_void, resource: *const i8, key_type: u32, error_code: *mut i32) -> *const c_void,
    pub set_key: extern "C" fn(priv_data: *const c_void, resource: *const i8, sub_song: u32, value: *const i8, key_type: *const i8) -> i32,
    pub set_key_with_encoding: extern "C" fn(priv_data: *const c_void, resource: *const i8, sub_song: u32, value: *const i8, key_type: u32, encoding: u32) -> i32,
    pub priv_data: *const c_void,
}

#[repr(C)]
#[derive(Debug)]
pub struct CMessageAPI {
    pub priv_data: *const c_void,
    pub begin_request: extern "C" fn(priv_data: *const c_void, id: *const i8) -> *const c_void,
    pub end_request: extern "C" fn(priv_data: *const c_void, message: *const c_void),
}

#[repr(C)]
#[derive(Debug)]
pub struct CMessage {
    pub priv_data: *const c_void,
    pub get_id: extern "C" fn(priv_data: *const c_void) -> u32,
    pub write_blob: extern "C" fn(priv_data: *const c_void, data: *const c_void, len: u32) -> i32,
    pub write_array_count: extern "C" fn(priv_data: *const c_void, len: u32) -> i32,
    pub write_str: extern "C" fn(priv_data: *const c_void, data: *const c_char) -> i32,
}

#[repr(C)]
#[derive(Debug)]
pub struct CHippoServiceAPI {
    pub get_io_api: extern "C" fn(priv_data: *const c_void, version: i32) -> *const CHippoIoAPI,
    pub get_metadata_api: extern "C" fn(priv_data: *const c_void, version: i32) -> *const CMetadataAPI,
    pub get_message_api: extern "C" fn(priv_data: *const c_void, version: i32) -> *const CMessageAPI,
    pub priv_data: *const c_void,	// memory handle
}

#[derive(Clone, Debug)]
#[repr(C)]
pub struct CHippoPlaybackPlugin {
    pub api_version: u64,
    pub name: u64,
    pub version: u64,
    pub probe_can_play: extern "C" fn(data: *const u8, data_size: u32, buffer: *const i8, total_size: u64) -> i32,
    pub supported_extensions: extern "C" fn() -> *mut c_char,
    pub create: extern "C" fn(service_api: *const CHippoServiceAPI) -> *mut c_void,
    pub destroy: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub open: extern "C" fn(user_data: *mut c_void, buffer: *const i8) -> c_int,
    pub close: extern "C" fn(user_data: *mut c_void) -> c_int,
    pub read_data: extern "C" fn(user_data: *mut c_void, dest: *mut u8, size: u32) -> c_int,
    pub seek: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub dummy0: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub dummy1: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub dummy2: extern "C" fn(user_data: *mut c_void, ms: c_int) -> c_int,
    pub private_data: u64,
}

unsafe impl Sync for CHippoViewPlugin {}

#[derive(Clone, Debug)]
#[repr(C)]
pub struct CHippoViewPlugin {
    pub api_version: u64,
    pub name: *const u8,
    pub version: *const u8,
    pub create: Option<extern "C" fn(service: *const CHippoServiceAPI) -> *mut c_void>,
    pub setup_ui: Option<extern "C" fn(user_data: *mut c_void, ui: *const PUPluginUI)>,
    pub destroy: Option<extern "C" fn(user_data: *mut c_void)>,
    pub event: Option<extern "C" fn(event: u32)>,
}

