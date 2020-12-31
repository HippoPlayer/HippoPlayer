use crate::ffi::{
    HSSetting, HippoSettingsError, HippoSettingsError_DuplicatedId, HippoSettingsError_Ok,
};
use logger::*;
use std::collections::HashMap;
use std::ffi::CStr;
use std::os::raw::{c_char, c_int, c_void};
use std::ptr;

pub struct Settings {
    native_settings: *const HSSetting,
    native_count: usize,
    _fields: Vec<HSSetting>,
}

impl Settings {
    /// Will construct a settings object not initialize the fields
    fn new() -> Settings {
        Settings {
            native_settings: ptr::null_mut(),
            native_count: 0,
            _fields: Vec::new(),
        }
    }

    fn new_alloc_fields(settings: *const HSSetting, count: usize) -> Settings {
        Settings {
            native_settings: settings,
            native_count: count,
            _fields: unsafe { Vec::from_raw_parts(settings as *mut _, count, count) },
        }
    }
}

/// Settings for a specific plugin type
struct PluginTypeSettings {
    /// Template for the global settings (return from the plugin)
    global_settings_template: Settings,
    /// Template for the file type settings (return from the plugin)
    file_type_settings_template: Settings,
    /// List of file extensions and settings for each
    file_type_settings: HashMap<String, Settings>,
    /// Global settings for the plugin type
    global_settings: Settings,
}

pub struct PlaybackSettings {
    settings: HashMap<String, PluginTypeSettings>,
}

impl PluginTypeSettings {
    fn new() -> PluginTypeSettings {
        PluginTypeSettings {
            global_settings_template: Settings::new(),
            file_type_settings_template: Settings::new(),
            file_type_settings: HashMap::new(),
            global_settings: Settings::new(),
        }
    }
}

impl PlaybackSettings {
    pub fn new() -> PlaybackSettings {
        PlaybackSettings {
            settings: HashMap::new(),
        }
    }

    /// Register global settings for the plugin type
    fn register_global_settings(
        &mut self,
        id: &str,
        native_settings: *const HSSetting,
        count: usize,
    ) -> HippoSettingsError {
        if let Some(ps) = self.settings.get_mut(id) {
            ps.global_settings_template.native_settings = native_settings;
            ps.global_settings_template.native_count = count;
            ps.global_settings = Settings::new_alloc_fields(native_settings, count);
            HippoSettingsError_Ok
        } else {
            warn!("No instance of {} for settings?", id);
            HippoSettingsError_DuplicatedId
        }
    }

    /// Register file type settings for the plugin
    fn register_filetype_settings(
        &mut self,
        id: &str,
        native_settings: *const HSSetting,
        count: usize,
    ) -> HippoSettingsError {
        if let Some(ps) = self.settings.get_mut(id) {
            ps.file_type_settings_template.native_settings = native_settings;
            ps.file_type_settings_template.native_count = count;

            // Iterater over all the filetype settings and clone the settings for each of them
            for (_key, val) in &mut ps.file_type_settings {
                *val = Settings::new_alloc_fields(native_settings, count);
            }
            HippoSettingsError_Ok
        } else {
            warn!("No instance of {} for settings.", id);
            HippoSettingsError_DuplicatedId
        }
    }

    /// Registers extensions for a plugin type. These string with comma separated entries
    pub fn register_file_extensions(&mut self, id: &str, extensions: &str) {
        // we expect that this plugin hasn't been registered at this point, if it has been we bail
        if self.settings.contains_key(id) {
            warn!("Plugin {} has already been registered. Bailing", id);
            return;
        }

        let extensions: Vec<&str> = extensions.split(',').collect();
        let mut plugin_settings = PluginTypeSettings::new();

        for e in extensions {
            plugin_settings
                .file_type_settings
                .insert(e.to_owned(), Settings::new());
        }

        self.settings.insert(id.to_owned(), plugin_settings);
    }
}

pub unsafe extern "C" fn register_filetype_settings(
    priv_data: *mut c_void,
    name: *const c_char,
    settings: *const HSSetting,
    count: c_int,
) -> HippoSettingsError {
    let ps: &mut PlaybackSettings = &mut *(priv_data as *mut PlaybackSettings);
    let plugin_id = CStr::from_ptr(name);
    ps.register_filetype_settings(&plugin_id.to_string_lossy(), settings, count as usize)
}

pub unsafe extern "C" fn register_global_settings(
    priv_data: *mut c_void,
    name: *const c_char,
    settings: *const HSSetting,
    count: c_int,
) -> HippoSettingsError {
    let ps: &mut PlaybackSettings = &mut *(priv_data as *mut PlaybackSettings);
    let plugin_id = CStr::from_ptr(name);
    ps.register_global_settings(&plugin_id.to_string_lossy(), settings, count as usize)
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
