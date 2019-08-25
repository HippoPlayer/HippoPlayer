use std::os::raw::c_char;
use std::collections::HashMap;

/*
use hippo_api;
use messages;
use rodio;
use walkdir;

#[macro_use]
use serde_derive;
pub use rmp;
use rmp_rpc;
use serde;
use serde_json;
*/

mod audio;
mod playlist;
mod plugin_handler;
mod song_db;

mod service;
pub mod service_ffi;

pub use rmp as msgpack;

#[derive(Default, Debug)]
pub struct SongDb {
    data: HashMap<String, String>,
}

#[no_mangle]
/// Create new instance of the song db
pub extern fn hippo_song_db_new() -> *const SongDb {
    std::ptr::null()
    /*
    let mut db = Box::new(SongDb::default());
    db.data.insert("test".to_owned(), "test2".to_owned());
    Box::into_raw(db) as *const SongDb
    */
}

#[no_mangle]
/// Update the song db with a new entry
pub extern fn hippo_song_db_update(_db: *mut SongDb, _name: *const c_char) {
    /*
    let db = unsafe { &mut *_db };
    println!("{:?}", db);
    */
}

