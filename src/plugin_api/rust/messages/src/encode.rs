use msgpack;
use msgpack::Marker;
use msgpack::encode::ValueWriteError;
use rmps::Serializer;
use serde::Serialize;
use std::io;

use MsgType;

pub struct Message {
    pub data: Vec<u8>,
    pub header_size: usize,
}

impl Message {
    pub fn new(method: &str, id: u32, message_type: MsgType) -> Result<Message, ValueWriteError> {
        let mut data = Vec::with_capacity(128);

        let msg_type = match message_type {
            MsgType::Request => 0,
            MsgType::Response => 1,
            MsgType::Notification => 2,
        };

        msgpack::encode::write_array_len(&mut data, 4)?;
        msgpack::encode::write_uint(&mut data, msg_type)?;
        msgpack::encode::write_uint(&mut data, id as u64)?;
        msgpack::encode::write_str(&mut data, method)?;

        // This is to keep track of that the user acutally wrote some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        let header_size = data.len();

        Ok(Message {
            data,
            header_size,
        })
    }

    pub fn new_request(method: &str, id: u32) -> Result<Message, ValueWriteError> {
        Self::new(method, id, MsgType::Request)
    }

    pub fn new_nesponse(method: &str, id: u32) -> Result<Message, ValueWriteError> {
        Self::new(method, id, MsgType::Response)
    }

    pub fn new_notification(method: &str, id: u32) -> Result<Message, ValueWriteError> {
        Self::new(method, id, MsgType::Notification)
    }

    #[inline]
    pub fn write_str(&mut self, data: &str) -> Result<(), ValueWriteError> {
        msgpack::encode::write_str(&mut self.data, data)
    }

    #[inline]
    pub fn write_uint(&mut self, data: u64) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_uint(&mut self.data, data)
    }

    #[inline]
    pub fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_array_len(&mut self.data, size)
    }

    #[inline]
    pub fn write_map_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_map_len(&mut self.data, size)
    }

    pub fn write_blob(&mut self, data: &[u8]) {
    	for d in data {
    		self.data.push(*d);
    	}
    }

    pub fn serialize<T: Serialize>(&mut self, data: &T) -> Result<(), ::rmps::encode::Error> {
        data.serialize(&mut Serializer::new(self))
    }
}

impl io::Write for Message {
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

