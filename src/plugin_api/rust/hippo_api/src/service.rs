use std::os::raw::{c_void, c_uchar};
use ffi::HippoServiceAPI;

#[derive(Clone, Copy)]
pub struct Service {
    api: *const HippoServiceAPI,
}

impl Service {
    pub fn new(api: *const HippoServiceAPI) -> Service {
        Service { api }
    }
}
