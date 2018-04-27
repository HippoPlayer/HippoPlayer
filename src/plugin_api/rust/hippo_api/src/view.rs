use ffi::*;
use service::Service;

pub trait HippoView {
    fn new(service: &Service) -> Self;
    fn destroy(&mut self) { }
    fn event(&mut self) -> DebugState;
}

pub fn create_view_instance<T: View>(service_func: extern "C" fn(service: *const c_uchar)
                                                                   -> *mut c_void)
                                           -> *mut c_void {
    let service = Service { service_func: service_func };
    let instance = unsafe { transmute(Box::new(T::new(&service))) };
    instance
}

pub fn destroy_view_instance<T: View>(ptr: *mut c_void) {
    let view: &mut T = unsafe { &mut *(ptr as *mut T) };
    view.destroy();
    let _: Box<T> = unsafe { transmute(ptr) };
    // implicitly dropped
}

#[macro_export]
macro_rules! define_view_plugin {
    ($p_name:ident, $name:expr, $version:expr, $x:ty) => {
        static $p_name: CBackendCallbacks = HippoViewPlugin {
            name: $name as *const u8,
            name: $version as *const u8,
            create: Some(hippo_api::view::create_view_instance::<$x>),
            destroy: Some(hippo_api::view::destroy_view_instance::<$x>),
            event: Some(hippo_api::view::event_view_instance::<$x>),
        };
    }
}


