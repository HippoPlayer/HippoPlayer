use std::os::raw::c_char;
use std::collections::HashMap;

#[derive(Default, Debug)]
pub struct SongDb {
    data: HashMap<String, String>,
}

#[no_mangle]
/// Create new instance of the song db
pub extern fn hippo_song_db_new() -> *const SongDb {
    let mut db = Box::new(SongDb::default());
    db.data.insert("test".to_owned(), "test2".to_owned());
    Box::into_raw(db) as *const SongDb
}

#[no_mangle]
/// Update the song db with a new entry
pub extern fn hippo_song_db_update(_db: *mut SongDb, _name: *const c_char) {
    let db = unsafe { &mut *_db };
    println!("{:?}", db);
}

