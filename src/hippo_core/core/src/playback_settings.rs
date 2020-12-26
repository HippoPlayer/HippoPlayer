use crate::ffi::{HSSetting, HippoSettingsError};
use std::os::raw::{c_char, c_int, c_void};
use std::ffi::CStr;

pub struct PlaybackSettings {
    _dummy: u32,
}

impl PlaybackSettings {
    pub fn new() -> PlaybackSettings {
        PlaybackSettings { _dummy: 0 }
    }
}

pub unsafe extern "C" fn register_filetype_settings(
    _priv_data: *mut c_void,
    _name: *const c_char,
    _settings: *const HSSetting,
    _count: c_int,
) -> HippoSettingsError {
    let plugin_id = CStr::from_ptr(_name);

    println!("Register settings for {}", plugin_id.to_string_lossy());

    0
}

pub unsafe extern "C" fn register_global_settings(
    _priv_data: *mut c_void,
    _name: *const c_char,
    _settings: *const HSSetting,
    _count: c_int,
) -> HippoSettingsError {
    0
}

pub unsafe extern "C" fn get_string(
    _priv_data: *mut c_void,
    _id: c_int,
    _value: *mut c_char,
    _max_len: c_int,
) -> HippoSettingsError {
    0
}

pub unsafe extern "C" fn get_int(
    _priv_data: *mut c_void,
    _id: c_int,
    _value: *mut c_int,
) -> HippoSettingsError {
    0
}

pub unsafe extern "C" fn get_float(
    _priv_data: *mut c_void,
    _id: c_int,
    _value: *mut f32,
) -> HippoSettingsError {
    0
}


