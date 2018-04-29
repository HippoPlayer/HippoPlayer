use std::collections::HashMap;
use std::fs::File;
use std::fs;
use std::io;

use std::io::Read;
use rmp;

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

const REQUEST_MESSAGE: u64 = 0;
const RESPONSE_MESSAGE: u64 = 1;
const NOTIFICATION_MESSAGE: u64 = 2;

struct MessageApi {
    request_id: u32,
}

impl MessageApi {
    fn begin_request(&mut self, name: &str, size_hint: usize) -> Result<Message> {
        let mut message = Message::new(size_hint);

        // Construct a MessagePack-RPC message
        rmp::encode::write_array_len(&mut message.data, 4)?;
        rmp::encode::write_uint(&mut message.data, REQUEST_MESSAGE)?;
        rmp::encode::write_str(&mut message.data, name)?;

        // This is to keep track of that the user acutally write some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.len();
        Ok(message)
    }
}

pub struct Message {
    data: Vec<u8>
    header_size,
}

impl Message {
    fn new(size_hint: usize) -> Message {
        Message {
            data: Vec::with_capacity(size_hint),
            header_size: 0,
        }
    }

    fn write_str(&mut self, data: &str) -> Result<(), ValueWriteError> {
        rmp::encode::write_str(self.data, data)
    }

    fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        rmp::encode::write_array_len(self.data, data)
    }
}



