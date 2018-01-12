
// ***************************************************************
// AUTO-GENERATED! DO NOT EDIT!
// ***************************************************************

use ffi_gen::*;
use std::ffi::CStr;
use std::slice;
pub use ffi_gen::PUBase as PUBase;


use std::ffi::CString;

pub use ffi_gen::PURect as Rect;

#[derive(Clone)]
pub struct Widget {
    pub obj: Option<PUWidget>,
}

#[derive(Clone)]
pub struct PushButton {
    pub obj: Option<PUPushButton>,
}

#[derive(Clone)]
pub struct Painter {
    pub obj: Option<PUPainter>,
}

#[derive(Clone)]
pub struct ListWidgetItem {
    pub obj: Option<PUListWidgetItem>,
}

#[derive(Clone)]
pub struct ListWidget {
    pub obj: Option<PUListWidget>,
}

#[derive(Clone)]
pub struct Slider {
    pub obj: Option<PUSlider>,
}

#[derive(Clone)]
pub struct MainWindow {
    pub obj: Option<PUMainWindow>,
}

#[derive(Clone)]
pub struct Action {
    pub obj: Option<PUAction>,
}

#[derive(Clone)]
pub struct Url {
    pub obj: Option<PUUrl>,
}

#[derive(Clone)]
pub struct MimeData {
    pub obj: Option<PUMimeData>,
}

#[derive(Clone)]
pub struct Menu {
    pub obj: Option<PUMenu>,
}

#[derive(Clone)]
pub struct MenuBar {
    pub obj: Option<PUMenuBar>,
}

#[derive(Clone)]
pub struct Application {
    pub obj: Option<PUApplication>,
}

#[derive(Clone)]
pub struct PaintEvent {
    pub obj: Option<PUPaintEvent>,
}

#[derive(Clone)]
pub struct DragEnterEvent {
    pub obj: Option<PUDragEnterEvent>,
}

#[derive(Clone)]
pub struct DropEvent {
    pub obj: Option<PUDropEvent>,
}

#[derive(Clone)]
pub struct Layout {
    pub obj: Option<PULayout>,
}

#[derive(Clone)]
pub struct VBoxLayout {
    pub obj: Option<PUVBoxLayout>,
}

#[derive(Clone)]
pub struct HBoxLayout {
    pub obj: Option<PUHBoxLayout>,
}

pub trait LayoutType {
    fn get_layout_type_obj(&self) -> *const PUBase;
}

pub trait PaintDevice {
    fn get_paint_device_obj(&self) -> *const PUBase;
}

pub trait WidgetType {
    fn get_widget_type_obj(&self) -> *const PUBase;
}

impl Widget {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }
}

impl PaintDevice for Widget {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for Widget {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl PushButton {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }

    pub fn set_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn set_flat (&self, flat: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_flat)(obj.privd, flat);
        
        }
    }
}

impl PaintDevice for PushButton {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for PushButton {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl Painter {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn begin (&self, target: &PaintDevice) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).begin)(obj.privd, target.get_paint_device_obj() as *const PUBase);
        
        }
    }

    pub fn end (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).end)(obj.privd);
        
        }
    }

    pub fn draw_line (&self, x1: i32, y1: i32, x2: i32, y2: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).draw_line)(obj.privd, x1, y1, x2, y2);
        
        }
    }
}

impl ListWidgetItem {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn set_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn text (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).text)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }
}

impl ListWidget {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }

    pub fn add_item (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_item)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn item (&self, index: i32) -> Option<ListWidgetItem> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).item)(obj.privd, index);
          
            if ret_val.privd.is_null() {
                None
            } else {
                Some(ListWidgetItem { obj: Some(ret_val) })
            }
          
        
        }
    }

    pub fn selected_items (&self) -> Vec<ListWidgetItem> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).selected_items)(obj.privd);
          
            if ret_val.count == 0 {
                Vec::new()
            } else {
                let mut data = Vec::with_capacity(ret_val.count as usize);
                let slice = slice::from_raw_parts(ret_val.elements as *const PUListWidgetItem, ret_val.count as usize);

                for item in slice {
                    data.push(ListWidgetItem { obj: Some(*item) });
                }

                data
            }
          
        
        }
    }

    pub fn set_drag_enabled (&self, state: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_drag_enabled)(obj.privd, state);
        
        }
    }

    pub fn set_drop_indicator_shown (&self, state: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_drop_indicator_shown)(obj.privd, state);
        
        }
    }

    pub fn set_accept_drops (&self, state: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_accept_drops)(obj.privd, state);
        
        }
    }

    pub fn add_widget_item (&self, item: &ListWidgetItem) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_widget_item)(obj.privd, item.obj.unwrap().privd);
        
        }
    }
}

impl PaintDevice for ListWidget {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for ListWidget {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl Slider {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }
}

impl PaintDevice for Slider {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for Slider {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl MainWindow {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }

    pub fn is_animated (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).is_animated)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn menu_bar (&self) -> MenuBar {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).menu_bar)(obj.privd);
          
           MenuBar { obj: Some(ret_val) }

          
        
        }
    }

    pub fn set_central_widget (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_central_widget)(obj.privd, widget.get_widget_type_obj() as *const PUBase);
        
        }
    }
}

impl PaintDevice for MainWindow {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for MainWindow {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl Action {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn is_enabled (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).is_enabled)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn set_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }
}

impl Url {

    pub fn is_local_file (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).is_local_file)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn to_local_file (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).to_local_file)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }
}

impl MimeData {

    pub fn has_color (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).has_color)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn has_image (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).has_image)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn has_text (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).has_text)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn has_urls (&self) -> bool {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).has_urls)(obj.privd);
          
            ret_val
          
        
        }
    }

    pub fn urls (&self) -> Vec<Url> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).urls)(obj.privd);
          
            if ret_val.count == 0 {
                Vec::new()
            } else {
                let mut data = Vec::with_capacity(ret_val.count as usize);
                let slice = slice::from_raw_parts(ret_val.elements as *const PUUrl, ret_val.count as usize);

                for item in slice {
                    data.push(Url { obj: Some(*item) });
                }

                data
            }
          
        
        }
    }
}

impl Menu {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }

    pub fn add_action_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_action_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn add_action (&self, action: &Action) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_action)(obj.privd, action.obj.unwrap().privd);
        
        }
    }

    pub fn set_title (&self, title: &str) {
        let str_in_title_1 = CString::new(title).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_title)(obj.privd, str_in_title_1.as_ptr());
        
        }
    }
}

impl PaintDevice for Menu {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for Menu {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl MenuBar {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn show (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).show)(obj.privd);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const PUBase);
        
        }
    }

    pub fn add_menu (&self, menu: &Menu) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_menu)(obj.privd, menu.obj.unwrap().privd);
        
        }
    }
}

impl PaintDevice for MenuBar {
    fn get_paint_device_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl WidgetType for MenuBar {
    fn get_widget_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl Application {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn set_style (&self, style: &str) {
        let str_in_style_1 = CString::new(style).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_style)(obj.privd, str_in_style_1.as_ptr());
        
        }
    }

    pub fn exec (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).exec)(obj.privd);
        
        }
    }
}

impl PaintEvent {

    pub fn rect (&self) -> Rect {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).rect)(obj.privd);
          
           ret_val

          
        
        }
    }
}

impl DragEnterEvent {

    pub fn accept (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).accept)(obj.privd);
        
        }
    }
}

impl DropEvent {

    pub fn accept_proposed_action (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).accept_proposed_action)(obj.privd);
        
        }
    }

    pub fn mime_data (&self) -> MimeData {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).mime_data)(obj.privd);
          
           MimeData { obj: Some(ret_val) }

          
        
        }
    }
}

impl Layout {

    pub fn add_widget (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const PUBase);
        
        }
    }
}

impl LayoutType for Layout {
    fn get_layout_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl VBoxLayout {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn add_widget (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const PUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }
}

impl LayoutType for VBoxLayout {
    fn get_layout_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}

impl HBoxLayout {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn add_widget (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const PUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }
}

impl LayoutType for HBoxLayout {
    fn get_layout_type_obj(&self) -> *const PUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const PUBase
    }
}


#[macro_export]
macro_rules! set_current_row_changed_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, row: i32) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, row);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_current_row_changed_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_item_clicked_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, item: *const PUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, &ListWidgetItem { obj: Some(*(item as *const wrui::ffi_gen::PUListWidgetItem)) });
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_item_clicked_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_item_double_clicked_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, item: *const PUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, &ListWidgetItem { obj: Some(*(item as *const wrui::ffi_gen::PUListWidgetItem)) });
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_item_double_clicked_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_released_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_released_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_triggered_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_triggered_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_value_changed_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, value: i32) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, value);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_value_changed_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_drag_enter_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const wrui::wrui::PUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = DragEnterEvent { obj: Some(*(event as *const wrui::ffi_gen::PUDragEnterEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_drag_enter_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_drop_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const wrui::wrui::PUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = DropEvent { obj: Some(*(event as *const wrui::ffi_gen::PUDropEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_drop_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_paint_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const wrui::wrui::PUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = PaintEvent { obj: Some(*(event as *const wrui::ffi_gen::PUPaintEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const c_void {
         let t: *const c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_paint_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[derive(Copy, Clone)]
pub struct Ui {
    pu: *const PU
}

impl Ui {
    pub fn new(pu: *const PU) -> Ui { Ui { pu: pu } }

    pub fn create_widget(&self) -> Widget {
        Widget { obj: Some(unsafe { ((*self.pu).create_widget)((*self.pu).privd) }) }
    }

    pub fn create_push_button(&self) -> PushButton {
        PushButton { obj: Some(unsafe { ((*self.pu).create_push_button)((*self.pu).privd) }) }
    }

    pub fn create_painter(&self) -> Painter {
        Painter { obj: Some(unsafe { ((*self.pu).create_painter)((*self.pu).privd) }) }
    }

    pub fn create_list_widget_item(&self) -> ListWidgetItem {
        ListWidgetItem { obj: Some(unsafe { ((*self.pu).create_list_widget_item)((*self.pu).privd) }) }
    }

    pub fn create_list_widget(&self) -> ListWidget {
        ListWidget { obj: Some(unsafe { ((*self.pu).create_list_widget)((*self.pu).privd) }) }
    }

    pub fn create_slider(&self) -> Slider {
        Slider { obj: Some(unsafe { ((*self.pu).create_slider)((*self.pu).privd) }) }
    }

    pub fn create_main_window(&self) -> MainWindow {
        MainWindow { obj: Some(unsafe { ((*self.pu).create_main_window)((*self.pu).privd) }) }
    }

    pub fn create_action(&self) -> Action {
        Action { obj: Some(unsafe { ((*self.pu).create_action)((*self.pu).privd) }) }
    }

    pub fn create_menu(&self) -> Menu {
        Menu { obj: Some(unsafe { ((*self.pu).create_menu)((*self.pu).privd) }) }
    }

    pub fn create_menu_bar(&self) -> MenuBar {
        MenuBar { obj: Some(unsafe { ((*self.pu).create_menu_bar)((*self.pu).privd) }) }
    }

    pub fn create_application(&self) -> Application {
        Application { obj: Some(unsafe { ((*self.pu).create_application)((*self.pu).privd) }) }
    }

    pub fn create_v_box_layout(&self) -> VBoxLayout {
        VBoxLayout { obj: Some(unsafe { ((*self.pu).create_v_box_layout)((*self.pu).privd) }) }
    }

    pub fn create_h_box_layout(&self) -> HBoxLayout {
        HBoxLayout { obj: Some(unsafe { ((*self.pu).create_h_box_layout)((*self.pu).privd) }) }
    }

}
