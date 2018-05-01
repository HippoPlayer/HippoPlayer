//use std::os::raw::{c_uchar, c_void};
use ffi::CHippoServiceAPI;
use std::ffi::CString;
use ffi::*;

#[derive(Clone, Copy)]
pub struct Service {
    api: *const CHippoServiceAPI,
}

#[derive(Clone, Copy)]
pub struct MessageApi {
    api: *const CMessageAPI,
}

pub struct Message {
    api: *const CMessage,
}

impl MessageApi {
    // TODO: Proper results
    pub fn begin_request(&self, id: &str) -> Message {
        let c_id = CString::new(id).unwrap();

        unsafe {
            Message {
                api: ((*self.api).begin_request)((*self.api).priv_data, c_id.as_ptr()) as *const CMessage,
            }
        }
    }

    pub fn end_request(&self, message: Message) {
        unsafe {
            ((*self.api).end_request)((*self.api).priv_data, (*message.api).priv_data)
        }
    }
}

impl Service {
    pub fn new(api: *const CHippoServiceAPI) -> Service {
        Service { api }
    }

    pub fn message_api(&self) -> MessageApi {
        unsafe {
            MessageApi {
                // TODO: Proper API version
                api: ((*self.api).get_message_api)((*self.api).priv_data, 0),
            }
        }
    }
}
