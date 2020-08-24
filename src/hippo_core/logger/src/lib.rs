use std::fmt;
use std::os::raw::c_void;
use std::ffi::{CString, CStr};
use messages::*;
use std::path::PathBuf;

extern "C" {
    fn hippo_log_clear();
    fn hippo_log_send_messages(enable: bool);
    fn hippo_log_to_file(filename: *const i8, enable: bool);
    pub fn hippo_log_new_state() -> *mut c_void;
    pub fn hippo_log_delete_state(state: *mut c_void);
    pub fn hippo_log_set_base_name(logger: *mut c_void, name: *const i8);
    pub fn hippo_log_get_messages() -> *const i8;
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

// Turn on logging to file the first thing we do pretty much in the core so we get all data
// TODO: Cleanup this function but should be good enough for now
pub fn init_file_log(path: &PathBuf) {
    let dir = path.join("hippo_log.txt");

    if let Some(name) = dir.to_str() {
        if let Ok(c_filename) = CString::new(name) {
            unsafe { hippo_log_to_file(c_filename.as_ptr(), true) };
            return;
        }
    } else {
        println!("Strange path {:#?}", dir);
    }

    println!("FATAL ERROR: Unable to create logging to file. Can't find config directory to write to. Please report this.\n");
}

pub fn do_log(log_level: i32, format: fmt::Arguments) {
    let t = format!("{}", format);
    let c_to_print = CString::new(t).unwrap();

    unsafe {
        hippo_log(std::ptr::null_mut(), log_level, std::ptr::null(), 0, c_to_print.as_ptr());
    }
}

pub fn incoming_message(msg: &HippoMessage) {
    match msg.message_type() {
        MessageType::log_clear => unsafe { hippo_log_clear() },
        MessageType::log_send_messages => {
            let send_msgs = msg.message_as_log_send_messages().unwrap();
            unsafe { hippo_log_send_messages(send_msgs.enable()) };
        },

        MessageType::log_file => {
            let log_file = msg.message_as_log_file().unwrap();
            let filename = log_file.filename().unwrap();
            let c_filename = CString::new(filename).unwrap();

            unsafe { hippo_log_to_file(c_filename.as_ptr(), log_file.enable()) };
        },

        _ => (),
    }
}

//pub fn send_log_messages(_msg: &HippoMessage) -> Option<Box<[u8]>> {
pub fn send_log_messages() -> Option<Box<[u8]>> {
    let mut message = unsafe { hippo_log_get_messages() };

    if message == std::ptr::null() {
        return None;
    }

    let mut builder = messages::FlatBufferBuilder::new_with_capacity(8192);
    let mut log_messages = Vec::with_capacity(64);

    // collect all log messages
    loop {
        let c_str = unsafe { CStr::from_ptr(message) };
        log_messages.push(builder.create_string(&c_str.to_string_lossy()));
        message = unsafe { hippo_log_get_messages() };
        if message == std::ptr::null() {
            break;
        }
    }


    let strings_vec = builder.create_vector(&log_messages);

    let added_urls = HippoLogMessages::create(
        &mut builder,
        &HippoLogMessagesArgs {
            messages: Some(strings_vec),
        },
    );

    Some(HippoMessage::create_def(
        builder,
        MessageType::log_messages,
        added_urls.as_union_value(),
    ))
}


#[macro_export]
macro_rules! trace {
    ($($arg:tt)+) => ({
        $crate::do_log(HIPPO_LOG_TRACE, format_args!($($arg)*))
    });
}

#[macro_export]
macro_rules! warn {
    ($($arg:tt)+) => ({
        $crate::do_log(HIPPO_LOG_WARN, format_args!($($arg)*))
    });
}

#[macro_export]
macro_rules! error {
    ($($arg:tt)+) => ({
        $crate::do_log(HIPPO_LOG_ERROR, format_args!($($arg)*))
    });
}



