use msgpack;
use msgpack::encode::ValueWriteError;
use msgpack::Marker;
use rmps::Serializer;
use serde::Serialize;
use std::io;

//use num_integer::Integer;
use num_traits::cast::FromPrimitive;
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

        Ok(Message { data, header_size })
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
        let ret = msgpack::encode::write_str(&mut self.data, data);
        ret
    }

    #[inline]
    pub fn write_int<T: FromPrimitive + Into<i64>>(
        &mut self,
        data: T,
    ) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_sint(&mut self.data, data.into())
    }

    #[inline]
    pub fn write_uint<T: FromPrimitive + Into<u64>>(
        &mut self,
        data: T,
    ) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_uint(&mut self.data, data.into())
    }

    #[inline]
    pub fn write_key_value_str(&mut self, key0: &str, key1: &str) -> Result<(), ValueWriteError> {
        msgpack::encode::write_str(&mut self.data, key0)?;
        msgpack::encode::write_str(&mut self.data, key1)
    }

    #[inline]
    pub fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        msgpack::encode::write_array_len(&mut self.data, size)
    }

    #[inline]
    pub fn write_map_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        println!(">>>>>>>>>> write_map_len pos before {}", self.data.len());
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
