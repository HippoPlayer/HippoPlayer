use std::collections::{HashMap};
use std::fs::File;
use std::fs;
use std::io;
use std::io::Cursor;

use std::io::{Read, ErrorKind};
use msgpack;
use msgpack::Marker;
use msgpack::encode::{ValueWriteError, Error};
use msgpack::decode::{ValueReadError};
use std::borrow::Cow;
use std::ffi::{CStr};

pub struct IoApi {
    pub saved_allocs: HashMap<*const u8, Box<[u8]>>,
}

impl IoApi {
    pub fn exists(&self, target: &str) -> i32 {
        match fs::metadata(target) {
            Err(_e) => 0,
            Ok(res) => if res.is_file() { 1 } else { 0 },
        }
    }

    pub fn open(&self, stream: &str) -> io::Result<File> {
        File::open(stream)
    }

    pub fn read_file_to_memory(&self, filename: &str) -> io::Result<Box<[u8]>> {
        let mut file = File::open(filename)?;
        let size = file.metadata()?.len();
        let mut dest_mem = vec![0; size as usize].into_boxed_slice();
        file.read(&mut dest_mem)?;
        Ok(dest_mem)
    }
}

pub struct FileWrapper {
    pub file: fs::File,
}

pub const REQUEST_MESSAGE: u64 = 0;
//pub const RESPONSE_MESSAGE: u64 = 1;
pub const NOTIFICATION_MESSAGE: u64 = 2;

// Used to tag message as invalid
const INVALID_MESSAGE: u64 = 3;

//struct PluginSubscribe {
//   message_types: HashSet<'static str>,
//}

pub struct MessageEncode {
    pub data: Vec<u8>,
    message_type: u64,
    _id: u32,
    header_size: usize,
}

#[derive(Debug)]
pub struct MessageDecode {
	pub notifaction_id: u32,
	// Really as string but keep this way to allow easy acess on C side 
	pub method: Vec<u8>,
	pub cursor: Cursor<Vec<u8>>,
}

pub struct MessageApi {
    request_id: u32,
    //subscriptions: HashMap<u64, PluginSubscribe>,
    pub request_queue: Vec<Vec<u8>>,
    pub notifaction_queue: Vec<MessageEncode>,
    pub respones_queue: Vec<Box<MessageEncode>>,
}

impl MessageApi {
    pub fn new() -> MessageApi {
        MessageApi {
            request_id: 0,
            request_queue: Vec::new(),
            notifaction_queue: Vec::new(),
            respones_queue: Vec::new(),
        }
    }

    pub fn begin_request(&mut self, name: &str) -> Result<Box<MessageEncode>, ValueWriteError> {
        let mut message = MessageEncode::new(self.request_id, REQUEST_MESSAGE);

        message.write_array_len(4)?;
        message.write_uint(REQUEST_MESSAGE)?;
        message.write_uint(self.request_id as u64)?;
        message.write_str(name)?;

        self.request_id += 1;

        // This is to keep track of that the user acutally write some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.len();
        Ok(Box::new(message))
    }

    pub fn begin_notification(&mut self, name: &str) -> Result<Box<MessageEncode>, ValueWriteError> {
        let mut message = Box::new(MessageEncode::new(self.request_id, NOTIFICATION_MESSAGE));

        message.write_array_len(4)?;
        message.write_uint(NOTIFICATION_MESSAGE)?;
        message.write_uint(self.request_id as u64)?;
        message.write_str(name)?;

        // This is to keep track of that the user acutally wrote some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.len();
        Ok(message)
    }

    pub fn end_message(&mut self, message: &mut MessageEncode) {
    	let message_data = message.data.clone();

		match message.message_type {
			REQUEST_MESSAGE => self.request_queue.push(message_data), 
			// RESPONSE_MESSAGE => self.respones_queue.push(message_data), 
			// NOTIFICATION_MESSAGE => self.notifaction_queue.push(message_data), 
			e @ _ => println!("Tried to push message in unsupported type {}", e),
		}

        message.message_type = INVALID_MESSAGE;
    }

    pub fn clear_queues(&mut self) {
    	self.request_queue.clear();
    }
}


impl MessageEncode {
    pub fn new(id: u32, message_type: u64) -> MessageEncode {
        MessageEncode {
            data: Vec::new(),
            header_size: 0,
            message_type,
            _id: id,
        }
    }

    pub fn write_blob(&mut self, data: &[u8]) {
    	for d in data {
    		self.data.push(*d);
    	}
    }

    pub fn write_str(&mut self, data: &str) -> Result<(), ValueWriteError> {
    	if self.message_type != INVALID_MESSAGE {
            msgpack::encode::write_str(&mut self.data, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. MessageEncode has ended?")))
        }
    }

    pub fn write_uint(&mut self, data: u64) -> Result<Marker, ValueWriteError> {
    	if self.message_type != INVALID_MESSAGE {
            msgpack::encode::write_uint(&mut self.data, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. MessageEncode has ended?")))
        }
    }

    pub fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
    	if self.message_type != INVALID_MESSAGE {
            msgpack::encode::write_array_len(&mut self.data, size)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. MessageEncode has ended?")))
        }
    }
}

impl io::Write for MessageEncode {
	///
	/// Just using the write_blob API to add data to the stream
	/// TODO: Error handling
	///
	fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
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


impl MessageDecode {
    pub fn new(data: Vec<u8>) -> Result<Box<MessageDecode>, ValueReadError> {
		let mut cursor = Cursor::new(data);

		//
		// Parse the message 
		//

		println!("reading array len");

		let array_len = msgpack::decode::read_array_len(&mut cursor)?;

		if array_len != 4 {
            return Err(ValueReadError::InvalidDataRead(Error::new(ErrorKind::Other, "Message array count has to be 4")))
		}

		//
		// Make sure notification type is valid (can only be 0 -2)
		//
		
		println!("notifaction type");

		// TODO: don't unwrap
		let notification_type: u64 = msgpack::decode::read_int(&mut cursor).unwrap();

		if notification_type >= INVALID_MESSAGE {
           return Err(ValueReadError::InvalidDataRead(Error::new(ErrorKind::Other, "Invalid notification type")));
		}

		println!("request id type");

		//
		// Get notifcation id 
		//

		// TODO: don't unwrap
		let id: u32 = msgpack::decode::read_int(&mut cursor).unwrap();

		//
		// Decode string id 
		//

		let mut string_dest = vec![0; 1024];

		{	
			// TODO: Don't unwrap
			let _text_data = msgpack::decode::read_str(&mut cursor, &mut string_dest).unwrap();
		}

		Ok(Box::new(MessageDecode {
			notifaction_id: id,
			// TODO: Don't unwrap here.
			method: string_dest,
			cursor,
		}))
    }

	pub fn get_method<'a>(&'a self) -> Cow<'a, str> {
		unsafe { CStr::from_ptr(self.method.as_ptr() as *const i8).to_string_lossy() }
	}
}



