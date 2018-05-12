use ::serde::{Serialize};
use ::rmps::{Serializer};

use ffi::CHippoServiceAPI;
use std::ffi::CString;
use std::os::raw::c_void;
use ffi::*;

#[derive(Clone, Copy)]
pub struct Service {
    api: *const CHippoServiceAPI,
}

#[derive(Clone, Copy, Default)]
pub struct MessageApi {
    api: Option<*const CMessageAPI>,
}

pub struct Message {
    api: Option<*const CMessage>,
}

impl MessageApi {
    // TODO: Proper results
    pub fn begin_request(&self, id: &str) -> Message {
        let c_id = CString::new(id).unwrap();

        unsafe {
            let api = *self.api.as_ref().unwrap();
            Message {
                api: Some(((*api).begin_request)((*api).priv_data, c_id.as_ptr()) as *const CMessage),
            }
        }
    }

    pub fn begin_notification(&self, id: &str) -> Message {
        let c_id = CString::new(id).unwrap();

        unsafe {
            let api = *self.api.as_ref().unwrap();
            Message {
                api: Some(((*api).begin_notification)((*api).priv_data, c_id.as_ptr()) as *const CMessage),
            }
        }
    }

    pub fn end_message(&self, message: Message) {
        unsafe {
            let api = *self.api.as_ref().unwrap();
            ((*api).end_message)((*api).priv_data, (*message.api.unwrap()).priv_data)
        }
    }

    pub fn send_request<T: Serialize>(&self, id: &str, data: &T) -> u32 {
        let mut msgpack_data = Vec::new();
        let message = self.begin_request(id);
        let request_id = message.get_id();

        // TODO: How to handle errors here?
        data.serialize(&mut Serializer::new(&mut msgpack_data)).unwrap();

        message.write_blob(&msgpack_data);

        self.end_message(message);

        request_id
    }

    // helpers, should we have this here?

	pub fn next_song(&self) {
		println!("Requesting next song");
		let msg = self.begin_notification("hippo_playlist_next_song");
		msg.write_uint(0);
		self.end_message(msg)
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

impl Message {
    pub fn get_id(&self) -> u32 {
        unsafe {
            let api = *self.api.as_ref().unwrap();
            ((*api).get_id)((*api).priv_data)
        }
    }

    pub fn write_blob(&self, data: &[u8]) {
        unsafe {
            let api = *self.api.as_ref().unwrap();
            ((*api).write_blob)((*api).priv_data, data.as_ptr() as *const c_void, data.len() as u32);
        }
    }

    pub fn write_uint(&self, data: u64) {
        unsafe {
            let api = *self.api.as_ref().unwrap();
            ((*api).write_uint)((*api).priv_data, data);
        }
    }

    //pub write_blob: extern "C" fn(priv_data: *const c_void, data: *const c_void, len: u32) -> i32,

}

    /*

    : extern "C" fn(priv_data: *const c_void) -> u32,
    pub write_array_count: extern "C" fn(priv_data: *const c_void, len: u32) -> i32,
    pub write_str: extern "C" fn(priv_data: *const c_void, data: *const c_char) -> i32,
    */
