use crate::ffi::{
    HSSetting, HippoSettingsError, HippoSettingsError_DuplicatedId, HippoSettingsError_NotFound,
    HippoSettingsError_Ok, HS_BOOL_TYPE, HS_FLOAT_TYPE, HS_INTEGER_RANGE_TYPE, HS_INTEGER_TYPE,
    HS_STRING_RANGE_TYPE,
};

use logger::*;
use serde_derive::{Deserialize, Serialize};
use serde_json;
use std::collections::HashMap;
use std::ffi::CStr;
use std::fs::File;
use std::io::{Error, ErrorKind, Read, Write};
use std::mem;
use std::os::raw::{c_char, c_int, c_void};
use std::path::PathBuf;
use std::slice;

#[derive(Clone, Serialize, Deserialize)]
enum SerValue {
    NoSetting,
    FloatValue(f32),
    IntValue(i32),
    BoolValue(bool),
    StrValue(String),
}

#[derive(Serialize, Deserialize)]
struct SerSetting {
    id: String,
    value: SerValue,
}

impl SerSetting {
    fn new(id: &str, value: SerValue) -> SerSetting {
        SerSetting {
            id: id.to_owned(),
            value,
        }
    }
}

pub struct Settings {
    pub native_settings: *const HSSetting,
    pub native_count: usize,
    pub fields: Vec<HSSetting>,
}

impl Settings {
    fn new(settings: *const HSSetting, count: usize) -> Settings {
        let slice = unsafe { slice::from_raw_parts(settings as *mut _, count) };
        Settings {
            native_settings: settings,
            native_count: count,
            fields: slice.to_vec(),
        }
    }

    /// returns true if data has changed
    fn has_changed(&self) -> bool {
        if self.native_count == 0 {
            return false;
        }

        let bytes_size = mem::size_of::<HSSetting>() * self.native_count;
        let data = unsafe { slice::from_raw_parts(self.native_settings as *const u8, bytes_size) };
        let template =
            unsafe { slice::from_raw_parts(self.fields.as_ptr() as *const u8, bytes_size) };

        data != template
    }

    pub fn reset(&mut self) {
    	if self.native_count != 0 {
			let slice = unsafe { slice::from_raw_parts(self.native_settings, self.native_count) };
			self.fields.copy_from_slice(&slice);
    	}
    }
}

#[derive(Serialize, Deserialize)]
struct SerPluginTypeSettings {
    plugin_name: String,
    settings: Vec<SerSetting>,
}

impl SerPluginTypeSettings {
    fn new() -> SerPluginTypeSettings {
        SerPluginTypeSettings {
            plugin_name: String::new(),
            settings: Vec::new(),
        }
    }
}

pub struct PlaybackSettings {
    pub settings: HashMap<String, Settings>,
    pub selected_id: String,
}

impl PlaybackSettings {
    pub fn new() -> PlaybackSettings {
        PlaybackSettings {
            settings: HashMap::new(),
            selected_id: String::new(),
        }
    }

    /// Register settings for the plugin type
    fn register_settings(
        &mut self,
        id: &str,
        native_settings: *const HSSetting,
        count: usize,
    ) -> HippoSettingsError {
        if let Some(_ps) = self.settings.get(id) {
            println!("Trying to register settings for {} twice, skipping", id);
            HippoSettingsError_DuplicatedId
        } else {
            self.settings
                .insert(id.to_owned(), Settings::new(native_settings, count));
            HippoSettingsError_Ok
        }
    }

    fn build_ser_settings(settings: &Settings) -> Vec<SerSetting> {
        let mut ser_settings = Vec::new();
        let native_settings = unsafe {
            slice::from_raw_parts(settings.native_settings as *const _, settings.native_count)
        };

        let bytes_size = std::mem::size_of::<HSSetting>();

        for (s, t) in settings.fields.iter().zip(native_settings.iter()) {
            let t0 =
                unsafe { slice::from_raw_parts(mem::transmute::<_, *const u8>(t), bytes_size) };
            let t1 =
                unsafe { slice::from_raw_parts(mem::transmute::<_, *const u8>(s), bytes_size) };

            if t0 != t1 {
                let widget_id = unsafe { CStr::from_ptr(s.int_value.base.widget_id) };
                let widget_type = widget_id.to_string_lossy();
                println!("field that differs is {}", &widget_id.to_string_lossy());

                println!("id {}", widget_type);

                let t = unsafe {
                    match s.int_value.base.widget_type as u32 {
                        HS_FLOAT_TYPE => {
                            SerSetting::new(&widget_type, SerValue::FloatValue(s.float_value.value))
                        }
                        HS_INTEGER_TYPE => {
                            SerSetting::new(&widget_type, SerValue::IntValue(s.int_value.value))
                        }
                        HS_INTEGER_RANGE_TYPE => {
                            SerSetting::new(&widget_type, SerValue::IntValue(s.int_value.value))
                        }
                        HS_BOOL_TYPE => {
                            SerSetting::new(&widget_type, SerValue::BoolValue(s.bool_value.value))
                        }
                        HS_STRING_RANGE_TYPE => {
                            let value_us = CStr::from_ptr(s.string_fixed_value.value);
                            let value = value_us.to_string_lossy().to_string();
                            SerSetting::new(&widget_type, SerValue::StrValue(value))
                        }
                        t => {
                            warn!("Setting id {} unknown {}", t, widget_type);
                            SerSetting::new("", SerValue::NoSetting)
                        }
                    }
                };

                ser_settings.push(t);
            }
        }

        ser_settings
    }

    /// Builds data structure to serialize the plugin settings
    /// The way this code works is that it does mem compares of the template with
    /// the instance data, if it differs it needs to be serialized, otherwise we skip it
    fn build_serialize_data(&self) -> Vec<SerPluginTypeSettings> {
        let mut ser_plugin_settings = Vec::new();

        for (plugin_name, settings) in &self.settings {
            let mut ser_settings = SerPluginTypeSettings::new();

            if settings.has_changed() {
                println!("Settings for {} has changed", plugin_name);
                ser_settings.settings = Self::build_ser_settings(&settings);
            }

            if !ser_settings.settings.is_empty() {
                ser_settings.plugin_name = plugin_name.to_owned();
                ser_plugin_settings.push(ser_settings);
            }
        }

        ser_plugin_settings
    }

    fn write_internal(&self, path: &str) -> std::io::Result<usize> {
        let ser_data = self.build_serialize_data();
        let mut file = File::create(&path)?;

        let toml = match serde_json::to_string_pretty(&ser_data) {
            Err(e) => {
                return Err(Error::new(
                    ErrorKind::Other,
                    format!("Unable to write plugins file {}: {}", path, e),
                ))
            }
            Ok(v) => v,
        };

        file.write(toml.as_bytes())
    }

    fn read_to_file(path: &str) -> std::io::Result<String> {
        let mut data = String::new();
        let mut file = File::open(&path)?;
        file.read_to_string(&mut data)?;
        Ok(data)
    }

    fn find_id<'a>(data: &'a mut Settings, id: &str) -> Option<&'a mut HSSetting> {
        for s in &mut data.fields {
            let widget_id = unsafe { CStr::from_ptr(s.int_value.base.widget_id) };
            let widget_type = widget_id.to_string_lossy();

            if id == widget_type {
                return Some(s);
            }
        }

        None
    }

    fn get_string_range_value(s: &HSSetting, name: &str) -> *const c_char {
        let slice: &[crate::ffi::HSStringRangeValue] = unsafe {
            slice::from_raw_parts(
                s.string_fixed_value.values as *const _,
                s.string_fixed_value.values_count as usize,
            )
        };

        for v in slice {
            let sel = unsafe { CStr::from_ptr(v.value) };
            let sel_name = sel.to_string_lossy();

            if sel_name == name {
            	return v.value;
            }
        }

        std::ptr::null()
    }

    fn patch_data(data: &mut Settings, input_data: &Vec<SerSetting>) {
        for input in input_data {
            if let Some(wd) = Self::find_id(data, &input.id) {
                match input.value {
                    SerValue::FloatValue(v) => wd.float_value.value = v,
                    SerValue::IntValue(v) => wd.int_value.value = v,
                    SerValue::BoolValue(v) => wd.bool_value.value = v,
                    SerValue::StrValue(ref v) => {
                        wd.string_fixed_value.value = Self::get_string_range_value(wd, &v)
                    }
                    SerValue::NoSetting => (),
                }
            } else {
                warn!("Id: {} wasn't found in settings", input.id);
            }
        }
    }

    fn load_internal(&mut self, path: &str) {
        if !std::fs::metadata(path).is_ok() {
            return;
        }

        let data = Self::read_to_file(path).unwrap();

        let ser_settings: Vec<SerPluginTypeSettings> = match serde_json::from_str(&data) {
            Err(e) => {
                error!("Unable to parse {} : {}", path, e);
                Vec::new()
            }
            Ok(v) => v,
        };

        for s in ser_settings {
            if let Some(ps) = self.settings.get_mut(&s.plugin_name) {
                Self::patch_data(ps, &s.settings);
            } else {
                warn!(
                    "Plugin {} not loaded, setting will be dropped",
                    s.plugin_name
                );
            }
        }
    }

    pub fn load(&mut self, path: &PathBuf, filename: &str) {
        let dir = path.join(filename);
        self.load_internal(&dir.to_string_lossy())
    }

    pub fn write(&self, path: &PathBuf, filename: &str) -> std::io::Result<usize> {
        let dir = path.join(filename);
        self.write_internal(&dir.to_string_lossy())
    }
}

pub unsafe extern "C" fn register_settings(
    priv_data: *mut c_void,
    name: *const c_char,
    settings: *const HSSetting,
    count: c_int,
) -> HippoSettingsError {
    let ps: &mut PlaybackSettings = &mut *(priv_data as *mut PlaybackSettings);
    let plugin_id = CStr::from_ptr(name);
    ps.register_settings(&plugin_id.to_string_lossy(), settings, count as usize)
}

unsafe fn find_setting<'a>(priv_data: *mut c_void, id: *const c_char) -> Option<&'a HSSetting> {
    let find_id = CStr::from_ptr(id);
    let search_id = find_id.to_string_lossy();

    let ps: &PlaybackSettings = &*(priv_data as *const PlaybackSettings);

    if let Some(settings) = ps.settings.get(&ps.selected_id) {
        for s in &settings.fields {
            let widget_id = CStr::from_ptr(s.int_value.base.widget_id);
            let widget_type = widget_id.to_string_lossy();

            if widget_type == search_id {
                return Some(s);
            }
        }
    }

    None
}

pub unsafe extern "C" fn get_string(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut *const c_char,
) -> HippoSettingsError {
    if let Some(setting) = find_setting(priv_data, id) {
        *value = setting.string_fixed_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_int(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut c_int,
) -> HippoSettingsError {
    if let Some(setting) = find_setting(priv_data, id) {
        *value = setting.int_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_float(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut f32,
) -> HippoSettingsError {
    if let Some(setting) = find_setting(priv_data, id) {
        *value = setting.float_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_bool(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut bool,
) -> HippoSettingsError {
    if let Some(setting) = find_setting(priv_data, id) {
        *value = setting.bool_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn register_settings_thread(
    _priv_data: *mut c_void,
    _name: *const c_char,
    _settings: *const HSSetting,
    _count: c_int,
) -> HippoSettingsError {
    warn!("Registering settings not supported from audio thread");
    HippoSettingsError_DuplicatedId
}

unsafe fn find_setting_thread<'a>(
    priv_data: *mut c_void,
    id: *const c_char,
) -> Option<&'a HSSetting> {
    let find_id = CStr::from_ptr(id);
    let search_id = find_id.to_string_lossy();

    let settings: &Vec<HSSetting> = &*(priv_data as *const Vec<HSSetting>);

    for s in settings {
        let widget_id = CStr::from_ptr(s.int_value.base.widget_id);
        let widget_type = widget_id.to_string_lossy();

        if widget_type == search_id {
            return Some(s);
        }
    }

    None
}

pub unsafe extern "C" fn get_string_thread(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut *const c_char,
) -> HippoSettingsError {
    if let Some(setting) = find_setting_thread(priv_data, id) {
        *value = setting.string_fixed_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_int_thread(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut c_int,
) -> HippoSettingsError {
    if let Some(setting) = find_setting_thread(priv_data, id) {
        *value = setting.int_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_float_thread(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut f32,
) -> HippoSettingsError {
    if let Some(setting) = find_setting_thread(priv_data, id) {
        *value = setting.float_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub unsafe extern "C" fn get_bool_thread(
    priv_data: *mut c_void,
    _ext: *const c_char,
    id: *const c_char,
    value: *mut bool,
) -> HippoSettingsError {
    if let Some(setting) = find_setting_thread(priv_data, id) {
        *value = setting.bool_value.value;
        HippoSettingsError_Ok
    } else {
        HippoSettingsError_NotFound
    }
}

pub fn get_threaded_callback(callback_data: *const c_void) -> crate::ffi::HippoSettingsAPI {
    crate::ffi::HippoSettingsAPI {
        priv_data: callback_data as *mut _,
        register_settings: Some(register_settings_thread),
        get_string: Some(get_string_thread),
        get_int: Some(get_int_thread),
        get_float: Some(get_float_thread),
        get_bool: Some(get_bool_thread),
    }
}
