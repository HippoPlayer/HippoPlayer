use ::serde::{Serialize};
use ::rmps::{Serializer};

use ffi::CHippoServiceAPI;
use std::ffi::CString;
use std::os::raw::c_void;
use ffi::*;
use std::io;

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
        //let mut msgpack_data = Vec::new();
        let mut message = self.begin_request(id);
        let request_id = message.get_id();

        // TODO: How to handle errors here?
        data.serialize(&mut Serializer::new(&mut message)).unwrap();

        self.end_message(message);

        request_id
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
}

impl io::Write for Message {
	///
	/// Just using the write_blob API to add data to the stream
	/// TODO: Error handling
	///
	fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
		println!("Message::write len {}", buf.len());
		self.write_blob(buf);
		Ok(buf.len())
	}

	///
	/// No need to flush here
	///
	fn flush(&mut self) -> io::Result<()> {
		Ok(())
	}
}


    /*

    : extern "C" fn(priv_data: *const c_void) -> u32,
    pub write_array_count: extern "C" fn(priv_data: *const c_void, len: u32) -> i32,
    pub write_str: extern "C" fn(priv_data: *const c_void, data: *const c_char) -> i32,
    */
