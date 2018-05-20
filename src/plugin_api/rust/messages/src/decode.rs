
use msgpack;

use std::io::Cursor;
use std::io::{ErrorKind};
use msgpack::encode::{Error};
use msgpack::decode::{NumValueReadError};
use std::str;

use serde::Deserialize;
use rmps::Deserializer;

const INVALID_MESSAGE: u64 = 3;

pub struct Message<'a> {
    pub data: Cursor<&'a [u8]>,
    pub method: &'a str,
    pub id: u32,
}

impl <'a>Message<'a> {
    pub fn new(data: &'a [u8]) -> Result<Message, NumValueReadError> {
		let mut cursor = Cursor::new(data);

		//
		// Parse the message
		//

		let array_len = msgpack::decode::read_array_len(&mut cursor)?;

		if array_len != 4 {
            return Err(NumValueReadError::InvalidDataRead(Error::new(ErrorKind::Other, "Message array count has to be 4")))
		}

		//
		// Make sure notification type is valid (can only be 0 -2)
		//

		let notification_type: u64 = msgpack::decode::read_int(&mut cursor)?;

		if notification_type >= INVALID_MESSAGE {
           return Err(NumValueReadError::InvalidDataRead(Error::new(ErrorKind::Other, "Invalid notification type")));
		}

		//
		// Get notifcation id
		//

		let id: u32 = msgpack::decode::read_int(&mut cursor)?;

		//
		// Decode string id
		//
		//
        let text_len = msgpack::decode::read_str_len(&mut cursor)? as usize;

        let current_pos = cursor.position() as usize;
        let new_pos = current_pos + text_len;

        let method = str::from_utf8(&data[current_pos..new_pos).map_err(|_|
           NumValueReadError::InvalidDataRead(Error::new(ErrorKind::Other, "Unable to decode method")))?;

        cursor.set_position(new_pos as u64);

		Ok(Message {
			data: cursor,
			method,
			id,
		})
    }

	///
	/// Deserializes using msgpack-serde deserialize
	///
	pub fn deserialize<T: Deserialize<'a>>(&mut self) -> Result<T, ::rmps::decode::Error> {
		let mut de = Deserializer::new(&mut self.data);
		let res: T = Deserialize::deserialize(&mut de)?;
		Ok(res)
    }
}


