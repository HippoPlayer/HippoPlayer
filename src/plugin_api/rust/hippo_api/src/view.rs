use service::Service;
use wrui::ffi_gen::PUPluginUI;
use wrui::wrui::PluginUi;
use std::os::raw::{c_uchar, c_void};
use std::mem::transmute;
use ffi::{CHippoServiceAPI};

pub trait View {
    fn new(service: &Service) -> Self;
    fn setup_ui(&mut self, ui: PluginUi);
    fn destroy(&mut self) {}
}

pub extern "C" fn create_view_instance<T: View>(service_api: *const CHippoServiceAPI) -> *mut c_void {
    let service = Service::new(service_api);
    let instance = unsafe { transmute(Box::new(T::new(&service))) };
    instance
}

pub extern "C" fn setup_ui_view_instance<T: View>(user_data: *mut c_void, ui: *const PUPluginUI) {
    let view: &mut T = unsafe { &mut *(user_data as *mut T) };
    let plugin_ui = PluginUi::new(ui);
    view.setup_ui(plugin_ui)
}

pub extern "C" fn destroy_view_instance<T: View>(ptr: *mut c_void) {
    let view: &mut T = unsafe { &mut *(ptr as *mut T) };
    view.destroy();
    let _: Box<T> = unsafe { transmute(ptr) };
    // implicitly dropped
}

#[macro_export]
macro_rules! define_view_plugin {
    ($p_name:ident, $name:expr, $version:expr, $x:ty) => {
        static $p_name: hippo_api::ffi::CHippoViewPlugin = hippo_api::ffi::CHippoViewPlugin {
            api_version: 0,
            name: $name as *const u8,
            version: $version as *const u8,
            create: Some(hippo_api::view::create_view_instance::<$x>),
            setup_ui: Some(hippo_api::view::setup_ui_view_instance::<$x>),
            destroy: Some(hippo_api::view::destroy_view_instance::<$x>),
            // event: Some(hippo_api::view::event_view_instance::<$x>),
            //save: None,
            //load: None,
            event: None,
        };

        //let ret: *const std::os::raw::c_void = unsafe { std::mem::transmute(&$p_name) };
        //ret
    }
}
