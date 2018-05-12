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

pub struct MessageApi {
    request_id: u32,
    //subscriptions: HashMap<u64, PluginSubscribe>,
    request_queue: Vec<Vec<u8>>,
    notifaction_queue: Vec<Message>,
    respones_queue: Vec<Box<Message>>,
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

    pub fn begin_request(&mut self, name: &str) -> Result<Box<Message>, ValueWriteError> {
        let mut message = Message::new(self.request_id);

        message.write_array_len(4)?;
        message.write_uint(REQUEST_MESSAGE)?;
        message.write_str(name)?;

        self.request_id += 1;

        // This is to keep track of that the user acutally write some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.as_ref().unwrap().len();
        Ok(message)
    }

    pub fn begin_notification(&mut self, name: &str) -> Result<Box<Message>, ValueWriteError> {
        let mut message = Message::new(self.request_id);

        message.write_array_len(4)?;
        message.write_uint(NOTIFICATION_MESSAGE)?;
        message.write_str(name)?;

        // This is to keep track of that the user acutally wrote some data. Otherwise we add
        // something dummy to make sure that we have a valid message
        message.header_size = message.data.as_ref().unwrap().len();
        Ok(message)
    }


    // TODO: Remove the copy here by using a linear allocator to fill in the memory
    pub fn end_message(&mut self, message: &mut Message) {
        self.request_queue.push(message.data.as_ref().unwrap().clone());
        // Mark the message as retired
        message.data = None;
    }

    // helpers, should we really have this here?
}


impl Message {
    fn new(id: u32) -> Box<Message> {
        Box::new(Message {
            data: Some(Vec::new()),
            header_size: 0,
            id,
        })
    }

    pub fn write_str(&mut self, data: &str) -> Result<(), ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            println!("message:write_str {}", data);
            msgpack::encode::write_str(dest, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }

    pub fn write_uint(&mut self, data: u64) -> Result<Marker, ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            msgpack::encode::write_uint(dest, data)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }

    pub fn write_array_len(&mut self, size: u32) -> Result<Marker, ValueWriteError> {
        if let Some(ref mut dest) = self.data {
            println!("message:write_array_len {}", size);
            msgpack::encode::write_array_len(dest, size)
        } else {
            Err(ValueWriteError::InvalidDataWrite(Error::new(ErrorKind::Other, "Not possible to write. Message has ended?")))
        }
    }
}



