use std::collections::{HashMap};
use std::fs::File;
use std::fs;
use std::io;


use std::io::{Read, ErrorKind};
use msgpack;
use msgpack::Marker;
use msgpack::encode::{ValueWriteError, Error};

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

//struct PluginSubscribe {
//   message_types: HashSet<'static str>,
//}

pub struct Message {
    data: Option<Vec<u8>>,
    id: u32,
    header_size: usize,
}

struct MessageApi {
    request_id: u32,
    //subscriptions: HashMap<u64, PluginSubscribe>,
    request_queue: Vec<Vec<u8>>,
    notifaction_queu: Vec<Message>,
    respones_queue: Vec<Message>,
}

impl MessageApi {
    fn begin_request(&mut self, name: &str, size_hint: usize) -> Result<Message, ValueWriteError> {
        let mut message = Message::new(size_hint, self.request_id);

        message.write_array_len(4)?;
        message.write_uint(REQUEST_MESSAGE)?;
        message.write_str(name)?;

        self.request_id += 1;

        // This is to keep track of that the user acutally write some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.as_ref().unwrap().len();
        Ok(message)
    }

    // TODO: Remove the copy here by using a linear allocator to fill in the memory
    fn end_request(&mut self, message: &mut Message) {
        self.request_queue.push(message.data.as_ref().unwrap().clone());
        // Mark the message as retired
        message.data = None;
    }
}


impl Message {
    fn new(size_hint: usize, id: u32) -> Message {
        Message {
            data: Some(Vec::with_capacity(size_hint)),
            header_size: 0,
            id,
        }
    }

    fn write_str(&mut self, data: &str) -> Result<(), ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            msgpack::encode::write_str(dest, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }

    fn write_uint(&mut self, data: u64) -> Result<Marker, ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            msgpack::encode::write_uint(dest, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }

    fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            msgpack::encode::write_array_len(dest, size)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }
}



