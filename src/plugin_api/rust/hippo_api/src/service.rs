use std::os::raw::{c_uchar, c_void};
use ffi::CHippoServiceAPI;

#[derive(Clone, Copy)]
pub struct Service {
    api: *const CHippoServiceAPI,
}

impl Service {
    pub fn new(api: *const CHippoServiceAPI) -> Service {
        Service { api }
    }
}
