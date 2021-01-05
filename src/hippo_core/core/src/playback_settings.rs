use crate::ffi::{
    HSSetting, HippoSettingsError, HippoSettingsError_DuplicatedId, HippoSettingsError_Ok,
    HS_FLOAT_TYPE, HS_INTEGER_TYPE, HS_BOOL_TYPE, HS_INTEGER_RANGE_TYPE, //HS_STRING_RANGE_TYPE,
};

use std::io::{Error, ErrorKind, Write/*Read*/};
use std::path::PathBuf;
use serde_derive::{Deserialize, Serialize};
use logger::*;
use std::collections::HashMap;
use std::ffi::CStr;
use std::os::raw::{c_char, c_int, c_void};
use std::slice;
use std::mem;
use std::fs::File;
use serde_json;

#[derive(Serialize, Deserialize)]
pub enum SerSetting {
    NoSetting,
    FloatValue(String, f32),
    IntValue(String, i32),
    IntRangeValue(String, i32),
    FloatRangeValue(String, i32),
    BoolValue(String, bool),
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
        let template = unsafe { slice::from_raw_parts(self.fields.as_ptr() as *const u8, bytes_size) };

        data != template
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
}

impl PlaybackSettings {
    pub fn new() -> PlaybackSettings {
        PlaybackSettings {
            settings: HashMap::new(),
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
            self.settings.insert(id.to_owned(), Settings::new(native_settings, count));
            HippoSettingsError_Ok
        }
    }

    fn build_ser_settings(settings: &Settings) -> Vec<SerSetting> {
        let mut ser_settings = Vec::new();
        let native_settings = unsafe { slice::from_raw_parts(settings.native_settings as *const _, settings.native_count) };

        let bytes_size = std::mem::size_of::<HSSetting>();

        for (s, t) in settings.fields.iter().zip(native_settings.iter()) {
            let t0 = unsafe { slice::from_raw_parts(mem::transmute::<_, *const u8>(t), bytes_size) };
            let t1 = unsafe { slice::from_raw_parts(mem::transmute::<_, *const u8>(s), bytes_size) };

            if t0 != t1 {
                let widget_id = unsafe { CStr::from_ptr(s.int_value.base.name) };
                let widget_type = widget_id.to_string_lossy();
                println!("field that differs is {}", &widget_id.to_string_lossy());

                let t = unsafe { match s.int_value.base.widget_type as u32 {
                    HS_FLOAT_TYPE => SerSetting::FloatValue(widget_type.into_owned(), s.float_value.value),
                    HS_INTEGER_TYPE => SerSetting::IntValue(widget_type.into_owned(), s.int_value.value),
                    HS_INTEGER_RANGE_TYPE => SerSetting::IntValue(widget_type.into_owned(), s.int_value.value),
                    HS_BOOL_TYPE => SerSetting::BoolValue(widget_type.into_owned(), s.bool_value.value),
                    t => {
                        warn!("Setting id {} unknown {}", t, widget_type);
                        SerSetting::NoSetting
                    }
                }};

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

    /*
    fn read_to_file(path: &str) -> std::io::Result<String> {
        let mut data = String::new();
        let mut file = File::open(&path)?;
        file.read_to_string(&mut data)?;
        Ok(data)
    }
    */

    /*
    fn load_internal(&mut self, path: &str) {
        if !std::fs::metadata(path).is_ok() {
            return;
        }

        let data = Self::read_to_file(path).unwrap();

        let t: Vec<SerPluginTypeSettings> = match serde_json::from_str(&data) {
            Err(e) => {
                error!("Unable to parse {} : {}", path, e);
                Vec::new()
            }
            Ok(v) => v,
        };

    }

    pub fn load(&mut self, path: &PathBuf, filename: &str) {
        let dir = path.join(filename);
        self.load_internal(&dir.to_string_lossy())
    }
    */

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

pub unsafe extern "C" fn get_string(
    _priv_data: *mut c_void,
    _ext: *const c_char,
    _id: *const c_char,
    _value: *mut c_char,
    _max_len: c_int,
) -> HippoSettingsError {
    0
}

pub unsafe extern "C" fn get_int(
    _priv_data: *mut c_void,
    _ext: *const c_char,
    _id: *const c_char,
    _value: *mut c_int,
) -> HippoSettingsError {
    0
}

pub unsafe extern "C" fn get_float(
    _priv_data: *mut c_void,
    _ext: *const c_char,
    _id: *const c_char,
    _value: *mut f32,
) -> HippoSettingsError {
    0
}
