use ffi::{CMessageAPI, CHippoServiceAPI};
use std::os::raw::c_void;

use messages;
use messages::ValueWriteError;

#[derive(Clone, Copy)]
pub struct Service {
    api: *const CHippoServiceAPI,
}

#[derive(Clone, Copy, Default)]
pub struct MessageApi {
    api: Option<*const CMessageAPI>,
}

impl MessageApi {
    pub fn begin_request(&mut self, name: &str) -> Result<Box<messages::encode::Message>, ValueWriteError> {
        let request_id;

        unsafe {
            let api = *self.api.as_ref().unwrap();
            request_id = ((*api).request_new_id)((*api).priv_data);
        }

        let message = messages::encode::Message::new_request(name, request_id)?;
        Ok(Box::new(message))
    }

    pub fn end_message(&self, message: Box<messages::encode::Message>) {
        unsafe {
            let t: *const c_void = ::std::mem::transmute(message);
            let api = *self.api.as_ref().unwrap();
            ((*api).end_message)((*api).priv_data, t)
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
                api: Some(((*self.api).get_message_api)((*self.api).priv_data, 0)),
            }
        }
    }
}

