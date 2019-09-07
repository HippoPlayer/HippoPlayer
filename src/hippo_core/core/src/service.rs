use std::collections::HashMap;
use std::fs;
use std::fs::File;
use std::io;

use std::io::Read;

use messages;
use messages::{MessageEncode, ValueWriteError};

pub struct IoApi {
    pub saved_allocs: HashMap<*const u8, Box<[u8]>>,
}

impl IoApi {
    pub fn exists(&self, target: &str) -> i32 {
        match fs::metadata(target) {
            Err(_e) => 0,
            Ok(res) => {
                if res.is_file() {
                    1
                } else {
                    0
                }
            }
        }
    }

    pub fn open(&self, stream: &str) -> io::Result<File> {
        File::open(stream)
    }

    pub fn read_file_to_memory(&self, filename: &str) -> io::Result<Box<[u8]>> {
        let mut file = File::open(filename)?;
        let size = file.metadata()?.len();
        let mut dest_mem = vec![0; size as usize].into_boxed_slice();
        file.read_exact(&mut dest_mem)?;
        Ok(dest_mem)
    }
}

pub struct FileWrapper {
    pub file: fs::File,
}

pub struct MessageApi {
    request_id: u32,
    pub request_queue: Vec<Box<messages::encode::Message>>,
}

impl MessageApi {
    pub fn new() -> MessageApi {
        MessageApi {
            request_id: 0,
            request_queue: Vec::new(),
        }
    }

    pub fn begin_request(&mut self, name: &str) -> Result<Box<MessageEncode>, ValueWriteError> {
        let message = MessageEncode::new_request(name, self.request_id)?;
        self.request_id += 1;
        Ok(Box::new(message))
    }

    pub fn begin_notification(
        &mut self,
        name: &str,
    ) -> Result<Box<MessageEncode>, ValueWriteError> {
        let message = MessageEncode::new_notification(name, self.request_id)?;
        self.request_id += 1;
        Ok(Box::new(message))
    }

    pub fn request_new_id(&mut self) -> u32 {
        let id = self.request_id;
        self.request_id += 1;
        id
    }

    // TODO: Fix me
    pub fn end_message(&mut self, message: Box<MessageEncode>) {
        self.request_queue.push(message);
    }

    pub fn clear_queues(&mut self) {
        self.request_queue.clear();
    }
}
