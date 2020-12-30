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

    fn register_filetype_settings(&mut self, id: &str, count: usize) {


    }
}



pub unsafe extern "C" fn register_filetype_settings(
    priv_data: *mut c_void,
    name: *const c_char,
    settings: *const HSSetting,
    count: c_int,
) -> HippoSettingsError {
    let settings: &mut PlaybackSettings = unsafe { &mut *(priv_data as *mut PlaybackSettings) };
    let plugin_id = CStr::from_ptr(name);

    //settings.register_filetype_settings(plugin_id.to_string_lossy(), count as usize);

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


