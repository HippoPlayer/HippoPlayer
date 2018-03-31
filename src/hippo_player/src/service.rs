use std::fs::File;
use std::os::raw::{c_int, c_void, c_char};
use std::ffi::CStr;
use std::fs;
use std::ptr;
use std::io;
use std::mem::transmute;
use std::slice;
use std::io::Read;
use std::collections::HashMap;
use song_db::SongDb;

struct IoApi {
    pub saved_allocs: HashMap<*const u8, Box<[u8]>>,
}

impl IoApi {
    fn exists(&self, target: &str) -> i32 {
        match fs::metadata(target) {
            Err(_e) => 0,
            Ok(res) => if res.is_file() { 1 } else { 0 },
        }
    }

    fn open(&self, stream: &str) -> io::Result<File> {
        File::open(stream)
    }

    fn read_file_to_memory(&self, filename: &str) -> io::Result<Box<[u8]>> {
        let mut file = File::open(filename)?;
        let size = file.metadata()?.len();
        let mut dest_mem = vec![0; size as usize].into_boxed_slice();
        file.read(&mut dest_mem)?;
        Ok(dest_mem)
    }
}

struct FileWrapper {
    file: fs::File,
}

#[repr(C)]
#[derive(Debug)]
pub struct CHippoIoAPI {
    pub exists: extern "C" fn(priv_data: *const c_void, target: *const u8) -> c_int,
    pub read_file_to_memory: extern "C" fn(priv_data: *const c_void, filename: *const u8, target: *mut *const c_void, target_size: *mut u64) -> i64,
    pub free_file_to_memory: extern "C" fn(priv_data: *const c_void, handle: *const c_void) -> i64,
    pub open: extern "C" fn(priv_data: *const c_void, target: *const u8, handle: *mut *const c_void) -> i64,
    pub close: extern "C" fn(handle: *const c_void) -> i64,
    pub size: extern "C" fn(handle: *const c_void, res: *mut u64) -> i64,
    pub read: extern "C" fn(handle: *const c_void, target: *mut u8, size: u64) -> i64,
    pub seek: extern "C" fn(handle: *const c_void, seek_type: i32, seek_step: u64) -> i64,
    pub priv_data: *const c_void,
}

#[repr(C)]
#[derive(Debug)]
pub struct CMetadataAPI {
    pub get_key: extern "C" fn(priv_data: *const c_void, resource: *const i8, key_type: u32, error_code: *mut i32) -> *const c_void,
    pub set_key: extern "C" fn(priv_data: *const c_void, resource: *const i8, sub_song: u32, value: *const i8, key_type: *const i8) -> i32,
    pub set_key_with_encoding: extern "C" fn(priv_data: *const c_void, resource: *const i8, sub_song: u32, value: *const i8, key_type: u32, encoding: u32) -> i32,
    pub priv_data: *const c_void,
}

#[repr(C)]
#[derive(Debug)]
pub struct CHippoServiceAPI {
    pub get_io_api: extern "C" fn(priv_data: *const c_void, version: i32) -> *const CHippoIoAPI,
    pub get_metadata_api: extern "C" fn(priv_data: *const c_void, version: i32) -> *const CMetadataAPI,
    pub private_data: *const c_void,	// memory handle
}

extern "C" fn file_exists_wrapper(priv_data: *const c_void, target: *const u8) -> i32 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let filename = unsafe { CStr::from_ptr(target as *const c_char) };
    file_api.exists(&filename.to_string_lossy())
}

extern "C" fn file_read_to_memory_wrapper(priv_data: *const c_void, filename: *const u8, target: *mut *const c_void, target_size: *mut u64) -> i64 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let name = unsafe { CStr::from_ptr(filename as *const c_char) };
    let fname = &name.to_string_lossy();

    unsafe {
        *target = ptr::null();
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

            let data_ptr: *const c_void = d.as_ptr() as *const c_void;

            unsafe {
                *target = data_ptr;
                *target_size = d.len() as u64;
            }
            0
        }
    }
}

extern "C" fn file_free_to_memory_wrapper(_priv_data: *const c_void, _handle: *const c_void) -> i64 {
    // implicitly dropped
    //let _: Box<[u8]> = unsafe { transmute(handle) };
    0
}

extern "C" fn file_open_wrapper(priv_data: *const c_void, target: *const u8, handle: *mut *const c_void) -> i64 {
    let file_api: &mut IoApi = unsafe { &mut *(priv_data as *mut IoApi) };
    let filename = unsafe { CStr::from_ptr(target as *const c_char) };

    // Handle fie

    unsafe { *handle = ptr::null() };

    match file_api.open(&filename.to_string_lossy()) {
        Err(e) => {
            // TODO: Proper error handling here
            println!("file open error {:?}", e);
            -1
        }

        Ok(f) => {
            println!("open file ok!");
            let file_wrapper: *const c_void = unsafe { transmute(Box::new(FileWrapper { file: f })) };
            unsafe { *handle = file_wrapper };
            0
        }
    }
}

extern "C" fn file_close_wrapper(_handle: *const c_void) -> i64 {
    // close here
    0
}

extern "C" fn file_size_wrapper(handle: *const c_void, size: *mut u64) -> i64 {
    let file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    // TODO: Error handling

    let file_size = file_wrapper.file.metadata().unwrap().len();
    unsafe { *size = file_size };

    0
}

extern "C" fn file_read_wrapper(handle: *const c_void, dest_data: *mut u8, size: u64) -> i64 {
    let file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    // TODO: Error handling

    let dest = unsafe { slice::from_raw_parts_mut(dest_data, size as usize) };

    file_wrapper.file.read(dest).unwrap();

    0
}

extern "C" fn file_seek_wrapper(handle: *const c_void, _seek_type: i32, _seek_step: u64) -> i64 {
    let _file_wrapper: &mut FileWrapper = unsafe { &mut *(handle as *mut FileWrapper) };
    0
}

extern "C" fn get_io_api_wrapper(priv_data: *const c_void, _version: i32) ->  *const CHippoIoAPI {
    let service_api: &mut ServiceApi = unsafe { &mut *(priv_data as *mut ServiceApi) };
    service_api.get_c_io_api()
}

extern "C" fn get_metadata_api(priv_data: *const c_void, _version: i32) ->  *const CMetadataAPI {
    let service_api: &mut ServiceApi = unsafe { &mut *(priv_data as *mut ServiceApi) };
    service_api.get_c_metadatao_api()
}


extern "C" fn metadata_get_key(_priv_data: *const c_void, _resource: *const i8, _key_type: u32, error_code: *mut i32) -> *const c_void {
    // not implemented yet
    unsafe { *error_code = -1; }
    ptr::null()
    //let res = unsafe { CStr::from_ptr(resource as *const c_char) };
    //file_api.exists(&filename.to_string_lossy())
}

extern "C" fn metadata_set_key(priv_data: *const c_void, resource: *const i8, sub_song: u32, value: *const i8, key: *const i8) -> i32 {
    let song_db: &mut SongDb = unsafe { &mut *(priv_data as *mut SongDb) };
    // TODO: Use CFixedString
    let res = unsafe { CStr::from_ptr(resource as *const c_char) };
    let value = unsafe { CStr::from_ptr(value as *const c_char) };
    let key = unsafe { CStr::from_ptr(key as *const c_char) };

    song_db.set_key( &res.to_string_lossy(), sub_song as usize, &value.to_string_lossy(), &key.to_string_lossy());

    0
}

extern "C" fn metadata_set_key_with_encoding(_priv_data: *const c_void, _resource: *const i8, _sub_song: u32, _value: *const i8, _key_type: u32, _encoding: u32) -> i32 {
    -1
}

pub struct ServiceApi {
    pub c_io_api: *const CHippoIoAPI,
    pub c_metadata_api: *const CMetadataAPI,
}

impl ServiceApi {
    fn get_c_io_api(&self) -> *const CHippoIoAPI {
        self.c_io_api
    }

    fn get_c_metadatao_api(&self) -> *const CMetadataAPI {
        self.c_metadata_api
    }

    fn get_song_db<'a>(&'a self) -> &'a SongDb {
    	let metadata_api = self.get_c_metadatao_api();
    	let song_db: &SongDb = unsafe { &*((*metadata_api).priv_data as *const SongDb) };
    	song_db
    }

    fn new() -> ServiceApi {
        // setup IO services

        let io_api: *const c_void = unsafe { transmute(Box::new(IoApi { saved_allocs: HashMap::new() })) };

        let c_io_api = Box::new(CHippoIoAPI {
           exists: file_exists_wrapper,
           read_file_to_memory: file_read_to_memory_wrapper,
           free_file_to_memory: file_free_to_memory_wrapper,
           open: file_open_wrapper,
           close: file_close_wrapper,
           size: file_size_wrapper,
           read: file_read_wrapper,
           seek: file_seek_wrapper,
           priv_data: io_api,
        });

        let c_io_api: *const CHippoIoAPI = unsafe { transmute(c_io_api) };

        // Metadata service

        let metadata_api: *const c_void = unsafe { transmute(Box::new(SongDb::new())) };

        let c_metadata_api = Box::new(CMetadataAPI {
            priv_data: metadata_api,
            get_key: metadata_get_key,
            set_key: metadata_set_key,
            set_key_with_encoding: metadata_set_key_with_encoding,
        });

        let c_metadata_api: *const CMetadataAPI = unsafe { transmute(c_metadata_api) };

        ServiceApi {
            c_io_api,
            c_metadata_api
        }
    }
}

pub struct PluginService {
    pub c_service_api: *const CHippoServiceAPI,
}

impl PluginService {
    pub fn new() -> PluginService {
        let service_api: *const c_void = unsafe { transmute(Box::new(ServiceApi::new())) };

        let c_service_api = Box::new(CHippoServiceAPI {
            get_io_api: get_io_api_wrapper,
            get_metadata_api: get_metadata_api,
            private_data: service_api,
        });

        let t: *const CHippoServiceAPI = unsafe { transmute(c_service_api) };

        PluginService {
            c_service_api: t
        }
    }

    pub fn get_song_db<'a>(&'a self) -> &'a SongDb {
    	let service_api: &ServiceApi = unsafe { &*((*self.c_service_api).private_data as *const ServiceApi) };
    	service_api.get_song_db()
    }

    pub fn get_c_service_api(&self) -> *const CHippoServiceAPI {
        self.c_service_api
    }
}

