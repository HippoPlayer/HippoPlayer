//use std::os::raw::{c_uchar, c_void};
use ffi::CHippoServiceAPI;

#[derive(Clone, Copy)]
pub struct Service {
    _api: *const CHippoServiceAPI,
}

impl Service {
    pub fn new(_api: *const CHippoServiceAPI) -> Service {
        Service { _api }
    }
}
