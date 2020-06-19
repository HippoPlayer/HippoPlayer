use crate::ffi;
use crate::service::{FileWrapper, IoApi, MessageApi};
use std::collections::HashMap;
use std::ffi::CStr;
use std::io::Read;
use std::mem::transmute;
use std::os::raw::{c_char, c_void};
use std::ptr;
use std::slice;
use song_db::*;

extern "C" fn file_exists_wrapper(priv_data: *mut ffi::HippoApiPrivData, target: *const i8) -> i32 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let filename = unsafe { CStr::from_ptr(target as *const c_char) };
    file_api.exists(&filename.to_string_lossy())
}

extern "C" fn file_read_to_memory_wrapper(
    priv_data: *mut ffi::HippoApiPrivData,
    filename: *const i8,
    target: *mut *mut c_void,
    target_size: *mut u64,
) -> i32 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let name = unsafe { CStr::from_ptr(filename as *const c_char) };
    let fname = &name.to_string_lossy();

    unsafe {
        *target = ptr::null_mut();
        *target_size = 0;
    }

    match file_api.read_file_to_memory(fname) {
        Err(e) => {
            println!("error reading {} error {:?}", fname, e);
            -1
        }

        Ok(data) => {
            let ptr = data.as_ptr();

            file_api.saved_allocs.insert(ptr, data);

            let d = file_api.saved_allocs.get(&ptr).unwrap();

            let data_ptr = d.as_ptr() as *mut c_void;

            unsafe {
                *target = data_ptr;
                *target_size = d.len() as u64;
            }
            0
        }
    }
}

extern "C" fn file_free_to_memory_wrapper(
    _priv_data: *mut ffi::HippoApiPrivData,
    _handle: *mut c_void,
) -> i32 {
    // implicitly dropped
    //let _: Box<[u8]> = unsafe { transmute(handle) };
    0
}

extern "C" fn file_open_wrapper(
    priv_data: *mut ffi::HippoApiPrivData,
    target: *const i8,
    handle: *mut *mut c_void,
) -> i32 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let filename = unsafe { CStr::from_ptr(target as *const c_char) };

    // Handle fie

    unsafe { *handle = ptr::null_mut() };

    match file_api.open(&filename.to_string_lossy()) {
        Err(e) => {
            // TODO: Proper error handling here
            println!("file open error {:?}", e);
            -1
        }

        Ok(f) => {
            let file_wrapper: *mut c_void = unsafe { transmute(Box::new(FileWrapper { file: f })) };
            unsafe { *handle = file_wrapper };
            0
        }
    }
}

extern "C" fn file_close_wrapper(_handle: ffi::HippoIoHandle) -> i32 {
    // close here
    0
}

extern "C" fn file_size_wrapper(handle: ffi::HippoIoHandle, size: *mut u64) -> i32 {
    let file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    // TODO: Error handling

    let file_size = file_wrapper.file.metadata().unwrap().len();
    unsafe { *size = file_size };

    0
}

extern "C" fn file_read_wrapper(
    handle: ffi::HippoIoHandle,
    dest_data: *mut c_void,
    size: u64,
) -> i32 {
    let file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    // TODO: Error handling

    let dest = unsafe { slice::from_raw_parts_mut(dest_data as *mut u8, size as usize) };

    file_wrapper.file.read_exact(dest).unwrap();

    0
}

extern "C" fn file_seek_wrapper(
    handle: ffi::HippoIoHandle,
    _seek_type: u32,
    _seek_step: i64,
) -> i32 {
    let _file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    0
}

extern "C" fn get_io_api_wrapper(
    priv_data: *mut ffi::HippoServicePrivData,
    _version: i32,
) -> *const ffi::HippoIoAPI {
    let service_api: &mut ServiceApi = unsafe { &mut *(priv_data as *mut ServiceApi) };
    service_api.get_c_io_api()
}

extern "C" fn get_metadata_api(
    priv_data: *mut ffi::HippoServicePrivData,
    _version: i32,
) -> *const ffi::HippoMetadataAPI {
    let service_api: &mut ServiceApi = unsafe { &mut *(priv_data as *mut ServiceApi) };
    service_api.get_c_metadata_api()
}

extern "C" fn get_message_api(
    priv_data: *mut ffi::HippoServicePrivData,
    _version: i32,
) -> *const ffi::HippoMessageAPI {
    let service_api: &mut ServiceApi = unsafe { &mut *(priv_data as *mut ServiceApi) };
    service_api.get_c_message_api()
}

extern "C" fn message_api_send(priv_data: *mut ffi::HippoMessageAPI, data: *const u8, len: i32) {
    let message_api: &mut MessageApi = unsafe { &mut *(priv_data as *mut MessageApi) };
    let data = unsafe { slice::from_raw_parts(data, len as usize) };
    message_api.send(data.to_vec().into_boxed_slice());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Wrappers for Metadata API
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" fn metadata_create_url(priv_data: *mut ffi::HippoMetadataAPIPrivData, url: *const i8) -> u64 {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    let url_str = unsafe { CStr::from_ptr(url) };

    match song_db.create_url(&url_str.to_string_lossy()) {
        Ok(v) => v,
        Err(err) => {
            println!("SongDb: create_url failed: {}", err);
            0
        }
    }
}

extern "C" fn metadata_set_tag(
    priv_data: *mut ffi::HippoMetadataAPIPrivData,
    url: u64,
    tag: *const c_char,
    data: *const c_char,
) {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };

    let tag_str = unsafe { CStr::from_ptr(tag) };
    let data_str = unsafe { CStr::from_ptr(data) };

    match song_db.set_tag_string(url, &tag_str.to_string_lossy(), &data_str.to_string_lossy()) {
        Err(err) => println!("SongDb: set_tag_string failed: {}", err),
        _ => (),
    }
}

extern "C" fn metadata_set_tag_f64(
    priv_data: *mut ffi::HippoMetadataAPIPrivData,
    url: u64,
    tag: *const c_char,
    data: f64
) {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    let tag_str = unsafe { CStr::from_ptr(tag) };

    match song_db.set_tag_float(url, &tag_str.to_string_lossy(), data as f32) {
        Err(err) => println!("SongDb: set_tag_f64 failed: {}", err),
        _ => (),
    }
}

extern "C" fn metadata_add_sub_song(
    priv_data: *mut ffi::HippoMetadataAPIPrivData,
    url: u64,
    name: *const c_char,
    length: f32
) {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    let name_str = unsafe { CStr::from_ptr(name) };

    match song_db.add_sub_song(url, &name_str.to_string_lossy(), length as f64) {
        Err(err) => println!("SongDb: add_sub_song failed: {}", err),
        _ => (),
    }
}

extern "C" fn metadata_add_sample(
    priv_data: *mut ffi::HippoMetadataAPIPrivData,
    url: u64,
    text: *const c_char
) {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    let text_str = unsafe { CStr::from_ptr(text) };

    match song_db.add_sample(url, &text_str.to_string_lossy()) {
        Err(err) => println!("SongDb: add_sample failed: {}", err),
        _ => (),
    }
}

extern "C" fn metadata_add_instrument(
    priv_data: *mut ffi::HippoMetadataAPIPrivData,
    url: u64,
    text: *const c_char
) {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    let text_str = unsafe { CStr::from_ptr(text) };

    match song_db.add_instrument(url, &text_str.to_string_lossy()) {
        Err(err) => println!("SongDb: add_instrument failed: {}", err),
        _ => (),
    }
}

pub struct ServiceApi {
    pub c_io_api: *const ffi::HippoIoAPI,
    pub c_metadata_api: *const ffi::HippoMetadataAPI,
    pub c_message_api: *const ffi::HippoMessageAPI,
}

pub fn new_c_message_api() -> *const ffi::HippoMessageAPI {
    let message_api = Box::into_raw(Box::new(MessageApi::new()));

    let c_message_api = Box::new(ffi::HippoMessageAPI {
        priv_data: message_api as *mut ffi::HippoMessageAPI,
        send: Some(message_api_send),
    });

    Box::into_raw(c_message_api) as *const ffi::HippoMessageAPI
}

impl ServiceApi {
    fn get_c_io_api(&self) -> *const ffi::HippoIoAPI {
        self.c_io_api
    }

    fn get_c_metadata_api(&self) -> *const ffi::HippoMetadataAPI {
        self.c_metadata_api
    }

    fn get_c_message_api(&self) -> *const ffi::HippoMessageAPI {
        self.c_message_api
    }

    fn get_song_db(&self) -> &SongDb {
        let metadata_api = self.get_c_metadata_api();
        let song_db: &SongDb = unsafe { &*((*metadata_api).priv_data as *const SongDb) };
        song_db
    }

    pub unsafe fn get_message_api_from_c_api(
        api: *const ffi::HippoMessageAPI,
    ) -> &'static MessageApi {
        let message_api: &MessageApi = &*((*api).priv_data as *const MessageApi);
        message_api
    }

    pub fn get_message_api_from_c_api_mut(
        api: *const ffi::HippoMessageAPI,
    ) -> &'static mut MessageApi {
        let message_api: &mut MessageApi = unsafe { &mut *((*api).priv_data as *mut MessageApi) };
        message_api
    }

    fn get_message_api(&self) -> &MessageApi {
        Self::get_message_api_from_c_api_mut(self.c_message_api)
    }

    fn get_message_api_mut(&mut self) -> &mut MessageApi {
        Self::get_message_api_from_c_api_mut(self.c_message_api)
    }

    fn new(song_db: *const SongDb) -> ServiceApi {
        // setup IO services

        let io_api: *mut ffi::HippoApiPrivData = unsafe {
            transmute(Box::new(IoApi {
                saved_allocs: HashMap::new(),
            }))
        };

        let c_io_api = Box::new(ffi::HippoIoAPI {
            exists: Some(file_exists_wrapper),
            read_file_to_memory: Some(file_read_to_memory_wrapper),
            free_file_to_memory: Some(file_free_to_memory_wrapper),
            open: Some(file_open_wrapper),
            close: Some(file_close_wrapper),
            size: Some(file_size_wrapper),
            read: Some(file_read_wrapper),
            seek: Some(file_seek_wrapper),
            priv_data: io_api,
        });

        let c_io_api: *const ffi::HippoIoAPI = unsafe { transmute(c_io_api) };

        // Metadata service

        //let metadata_api =
        //    Box::into_raw(Box::new(SongDb::new("songdb.db"))) as *mut ffi::HippoMetadataAPIPrivData;

        let c_metadata_api = Box::new(ffi::HippoMetadataAPI {
            priv_data: song_db as *const ffi::HippoMetadataAPIPrivData,
            create_url: Some(metadata_create_url),
            set_tag: Some(metadata_set_tag),
            set_tag_f64: Some(metadata_set_tag_f64),
            add_sub_song: Some(metadata_add_sub_song),
            add_sample: Some(metadata_add_sample),
            add_instrument: Some(metadata_add_instrument),
        });

        let c_metadata_api: *const ffi::HippoMetadataAPI = unsafe { transmute(c_metadata_api) };

        ServiceApi {
            c_io_api,
            c_metadata_api,
            c_message_api: new_c_message_api(),
        }
    }
}

pub struct PluginService {
    pub c_service_api: *const ffi::HippoServiceAPI,
}

impl PluginService {
    pub fn new(song_db: *const SongDb) -> PluginService {
        PluginService {
            c_service_api: Self::new_c_api(song_db),
        }
    }

    pub fn new_c_api(song_db: *const SongDb) -> *const ffi::HippoServiceAPI {
        let service_api = Box::into_raw(Box::new(ServiceApi::new(song_db)));

        let c_service_api = Box::new(ffi::HippoServiceAPI {
            get_io_api: Some(get_io_api_wrapper),
            get_metadata_api: Some(get_metadata_api),
            get_message_api: Some(get_message_api),
            private_data: service_api as *mut ffi::HippoServicePrivData,
        });

        Box::into_raw(c_service_api) as *const ffi::HippoServiceAPI
    }

    pub fn get_song_db<'a>(&'a self) -> &'a SongDb {
        let service_api: &ServiceApi =
            unsafe { &*((*self.c_service_api).private_data as *const ServiceApi) };
        service_api.get_song_db()
    }

    pub fn get_c_service_api(&self) -> *const ffi::HippoServiceAPI {
        self.c_service_api
    }

    pub fn get_message_api<'a>(&'a self) -> &'a MessageApi {
        let service_api: &ServiceApi =
            unsafe { &*((*self.c_service_api).private_data as *const ServiceApi) };
        service_api.get_message_api()
    }

    pub fn get_message_api_mut(&mut self) -> &mut MessageApi {
        let service_api: &mut ServiceApi =
            unsafe { &mut *((*self.c_service_api).private_data as *mut ServiceApi) };
        service_api.get_message_api_mut()
    }
}
