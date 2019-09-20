#![allow(clippy::box_vec)]

use std::collections::HashMap;
use std::fs;
use std::fs::File;
use std::io;
use std::io::Read;
use ringbuf;
use ringbuf::{RingBuffer};

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
    pub read_stream: ringbuf::Consumer<Box<[u8]>>,
    pub write_stream: ringbuf::Producer<Box<[u8]>>,
}

impl MessageApi {
    pub fn new() -> MessageApi {
        let rb = RingBuffer::<Box<[u8]>>::new(256);
        let (prod, cons) = rb.split();

        MessageApi {
            write_stream: prod,
            read_stream: cons,
        }
    }

    pub fn send(&mut self, message: Box<[u8]>) {
        //self.write_stream.push(message).unwrap();
        self.write_stream.push(message);
    }
}
