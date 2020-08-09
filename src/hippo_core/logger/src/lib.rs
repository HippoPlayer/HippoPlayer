use std::fmt;
use std::os::raw::c_void;
use std::ffi::CString;

extern "C" {
    pub fn hippo_log_new_state() -> *mut c_void;
    pub fn hippo_log_delete_state(state: *mut c_void);
    pub fn hippo_log_set_base_name(logger: *mut c_void, name: *const i8);
    pub fn hippo_log(
        logger: *mut c_void,
        level: i32,
        file: *const i8,
        line: i32,
        fmt: *const i8,
        ...
    );
}

pub const HIPPO_LOG_TRACE: i32 = 0;
pub const HIPPO_LOG_DEBUG: i32 = 1;
pub const HIPPO_LOG_INFO: i32 = 2;
pub const HIPPO_LOG_WARN: i32 = 3;
pub const HIPPO_LOG_ERROR: i32 = 4;
pub const HIPPO_LOG_FATAL: i32 = 5;

pub fn do_log(log_level: i32, format: fmt::Arguments) {
    let t = format!("{}", format);
    let c_to_print = CString::new(t).unwrap();
    unsafe {
        hippo_log(std::ptr::null_mut(), log_level, std::ptr::null(), 0, c_to_print.as_ptr());
    }
}

#[macro_export]
macro_rules! trace {
    ($($arg:tt)+) => ({
        $crate::do_log(HIPPO_LOG_TRACE, format_args!($($arg)*))
    });
}
