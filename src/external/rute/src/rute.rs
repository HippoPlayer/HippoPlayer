
// ***************************************************************
// AUTO-GENERATED! DO NOT EDIT!
// ***************************************************************

use ffi_gen::*;
use std::ffi::CStr;
use std::slice;
pub use ffi_gen::RUBase as RUBase;


use std::ffi::CString;

pub use ffi_gen::RUMetaKeys as MetaKeys;

pub use ffi_gen::RUKeys as Keys;

pub use ffi_gen::RURect as Rect;

pub use ffi_gen::RUColor as Color;

#[derive(Clone, Default)]
pub struct Widget {
    pub obj: Option<RUWidget>,
}

#[derive(Clone, Default)]
pub struct PushButton {
    pub obj: Option<RUPushButton>,
}

#[derive(Clone, Default)]
pub struct Painter {
    pub obj: Option<RUPainter>,
}

#[derive(Clone, Default)]
pub struct ListWidgetItem {
    pub obj: Option<RUListWidgetItem>,
}

#[derive(Clone, Default)]
pub struct ListWidget {
    pub obj: Option<RUListWidget>,
}

#[derive(Clone, Default)]
pub struct Label {
    pub obj: Option<RULabel>,
}

#[derive(Clone, Default)]
pub struct LineEdit {
    pub obj: Option<RULineEdit>,
}

#[derive(Clone, Default)]
pub struct PlainTextEdit {
    pub obj: Option<RUPlainTextEdit>,
}

#[derive(Clone, Default)]
pub struct Slider {
    pub obj: Option<RUSlider>,
}

#[derive(Clone, Default)]
pub struct MainWindow {
    pub obj: Option<RUMainWindow>,
}

#[derive(Clone, Default)]
pub struct ToolWindowManager {
    pub obj: Option<RUToolWindowManager>,
}

#[derive(Clone, Default)]
pub struct DockWidget {
    pub obj: Option<RUDockWidget>,
}

#[derive(Clone, Default)]
pub struct DockManager {
    pub obj: Option<RUDockManager>,
}

#[derive(Clone, Default)]
pub struct FramelessWindow {
    pub obj: Option<RUFramelessWindow>,
}

#[derive(Clone, Default)]
pub struct Action {
    pub obj: Option<RUAction>,
}

#[derive(Clone, Default)]
pub struct Url {
    pub obj: Option<RUUrl>,
}

#[derive(Clone, Default)]
pub struct MimeData {
    pub obj: Option<RUMimeData>,
}

#[derive(Clone, Default)]
pub struct Timer {
    pub obj: Option<RUTimer>,
}

#[derive(Clone, Default)]
pub struct Icon {
    pub obj: Option<RUIcon>,
}

#[derive(Clone, Default)]
pub struct Font {
    pub obj: Option<RUFont>,
}

#[derive(Clone, Default)]
pub struct Menu {
    pub obj: Option<RUMenu>,
}

#[derive(Clone, Default)]
pub struct MenuBar {
    pub obj: Option<RUMenuBar>,
}

#[derive(Clone, Default)]
pub struct Application {
    pub obj: Option<RUApplication>,
}

#[derive(Clone, Default)]
pub struct PaintEvent {
    pub obj: Option<RUPaintEvent>,
}

#[derive(Clone, Default)]
pub struct DragEnterEvent {
    pub obj: Option<RUDragEnterEvent>,
}

#[derive(Clone, Default)]
pub struct DropEvent {
    pub obj: Option<RUDropEvent>,
}

#[derive(Clone, Default)]
pub struct Layout {
    pub obj: Option<RULayout>,
}

#[derive(Clone, Default)]
pub struct VBoxLayout {
    pub obj: Option<RUVBoxLayout>,
}

#[derive(Clone, Default)]
pub struct HBoxLayout {
    pub obj: Option<RUHBoxLayout>,
}

pub trait LayoutType {
    fn get_layout_type_obj(&self) -> *const RUBase;
}

pub trait PaintDevice {
    fn get_paint_device_obj(&self) -> *const RUBase;
}

pub trait WidgetType {
    fn get_widget_type_obj(&self) -> *const RUBase;
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }
}

impl PaintDevice for Widget {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for Widget {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn set_icon (&self, icon: &Icon) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_icon)(obj.privd, icon.obj.unwrap().privd);
        
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
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for PushButton {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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
        
            ((*obj.funcs).begin)(obj.privd, target.get_paint_device_obj() as *const RUBase);
        
        }
    }

    pub fn end (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).end)(obj.privd);
        
        }
    }

    pub fn set_font (&self, font: &Font) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_font)(obj.privd, font.obj.unwrap().privd);
        
        }
    }

    pub fn draw_text (&self, x: i32, y: i32, text: &str) {
        let str_in_text_3 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).draw_text)(obj.privd, x, y, str_in_text_3.as_ptr());
        
        }
    }

    pub fn draw_line (&self, x1: i32, y1: i32, x2: i32, y2: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).draw_line)(obj.privd, x1, y1, x2, y2);
        
        }
    }

    pub fn fill_rect_color (&self, rect: Rect, color: Color) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).fill_rect_color)(obj.privd, rect, color);
        
        }
    }
}

impl Drop for Painter {
    fn drop(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd as *const RUBase)
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

    pub fn set_string_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_string_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn get_string_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).get_string_data)(obj.privd);
          
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn clear (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).clear)(obj.privd);
        
        }
    }

    pub fn add_item (&self, item: &ListWidgetItem) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_item)(obj.privd, item.obj.unwrap().privd);
        
        }
    }

    pub fn add_text_item (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_text_item)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn current_item (&self) -> Option<ListWidgetItem> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).current_item)(obj.privd);
          
            if ret_val.privd.is_null() {
                None
            } else {
                Some(ListWidgetItem { obj: Some(ret_val) })
            }
          
        
        }
    }

    pub fn current_row (&self) -> i32 {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).current_row)(obj.privd);
          
            ret_val
          
        
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
                let slice = slice::from_raw_parts(ret_val.elements as *const RUListWidgetItem, ret_val.count as usize);

                for item in slice {
                    data.push(ListWidgetItem { obj: Some(*item) });
                }

                data
            }
          
        
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

    pub fn set_current_row (&self, index: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_current_row)(obj.privd, index);
        
        }
    }

    pub fn count (&self) -> i32 {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).count)(obj.privd);
          
            ret_val
          
        
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
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for ListWidget {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl Label {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
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

impl PaintDevice for Label {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for Label {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl LineEdit {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn set_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn set_read_only (&self, value: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_read_only)(obj.privd, value);
        
        }
    }
}

impl PaintDevice for LineEdit {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for LineEdit {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl PlainTextEdit {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn clear (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).clear)(obj.privd);
        
        }
    }

    pub fn set_plain_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_plain_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn append_plain_text (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).append_plain_text)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn set_read_only (&self, value: bool) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_read_only)(obj.privd, value);
        
        }
    }
}

impl PaintDevice for PlainTextEdit {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for PlainTextEdit {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }
}

impl PaintDevice for Slider {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for Slider {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
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
        
            ((*obj.funcs).set_central_widget)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }
}

impl Drop for MainWindow {
    fn drop(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd as *const RUBase)
       }
    }
}

impl PaintDevice for MainWindow {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for MainWindow {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl ToolWindowManager {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn save_state (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).save_state)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn restore_state (&self, state: &str) {
        let str_in_state_1 = CString::new(state).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).restore_state)(obj.privd, str_in_state_1.as_ptr());
        
        }
    }

    pub fn add_to_docking (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_to_docking)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn add_to_docking_floating (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_to_docking_floating)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }
}

impl PaintDevice for ToolWindowManager {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for ToolWindowManager {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl DockWidget {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn set_object_name (&self, name: &str) {
        let str_in_name_1 = CString::new(name).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_object_name)(obj.privd, str_in_name_1.as_ptr());
        
        }
    }

    pub fn object_name (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).object_name)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_widget (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_widget)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }
}

impl DockManager {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn save_state (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).save_state)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn restore_state (&self, state: &str) -> bool {
        let str_in_state_1 = CString::new(state).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).restore_state)(obj.privd, str_in_state_1.as_ptr());
          
            ret_val
          
        
        }
    }

    pub fn add_to_docking (&self, widget: &DockWidget) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_to_docking)(obj.privd, widget.obj.unwrap().privd);
        
        }
    }

    pub fn get_dock_widgets (&self) -> Vec<DockWidget> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).get_dock_widgets)(obj.privd);
          
            if ret_val.count == 0 {
                Vec::new()
            } else {
                let mut data = Vec::with_capacity(ret_val.count as usize);
                let slice = slice::from_raw_parts(ret_val.elements as *const RUDockWidget, ret_val.count as usize);

                for item in slice {
                    data.push(DockWidget { obj: Some(*item) });
                }

                data
            }
          
        
        }
    }
}

impl PaintDevice for DockManager {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for DockManager {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl FramelessWindow {
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
        }
    }

    pub fn set_window_title (&self, title: &str) {
        let str_in_title_1 = CString::new(title).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_window_title)(obj.privd, str_in_title_1.as_ptr());
        
        }
    }

    pub fn set_content (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_content)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }
}

impl PaintDevice for FramelessWindow {
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for FramelessWindow {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn text (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).text)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_shortcut (&self, key: Keys) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_shortcut)(obj.privd, key);
        
        }
    }

    pub fn set_shortcut_mod (&self, key: Keys, modifier: MetaKeys) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_shortcut_mod)(obj.privd, key, modifier);
        
        }
    }

    pub fn set_int_data (&self, data: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_int_data)(obj.privd, data);
        
        }
    }

    pub fn get_int_data (&self) -> i32 {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).get_int_data)(obj.privd);
          
            ret_val
          
        
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
                let slice = slice::from_raw_parts(ret_val.elements as *const RUUrl, ret_val.count as usize);

                for item in slice {
                    data.push(Url { obj: Some(*item) });
                }

                data
            }
          
        
        }
    }
}

impl Timer {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn start (&self, time: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).start)(obj.privd, time);
        
        }
    }
}

impl Icon {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn add_file (&self, filename: &str) {
        let str_in_filename_1 = CString::new(filename).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_file)(obj.privd, str_in_filename_1.as_ptr());
        
        }
    }
}

impl Font {
    pub fn destroy(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd);
          self.obj = None;
       }
    }

    pub fn set_family (&self, family: &str) {
        let str_in_family_1 = CString::new(family).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_family)(obj.privd, str_in_family_1.as_ptr());
        
        }
    }

    pub fn set_point_size (&self, size: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_point_size)(obj.privd, size);
        
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
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
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for Menu {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn set_persist_data (&self, text: &str) {
        let str_in_text_1 = CString::new(text).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_persist_data)(obj.privd, str_in_text_1.as_ptr());
        
        }
    }

    pub fn persist_data (&self) -> String {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).persist_data)(obj.privd);
          
           CStr::from_ptr(ret_val).to_string_lossy().into_owned()
          
        
        }
    }

    pub fn set_fixed_height (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_height)(obj.privd, width);
        
        }
    }

    pub fn set_fixed_width (&self, width: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_fixed_width)(obj.privd, width);
        
        }
    }

    pub fn resize (&self, width: i32, height: i32) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).resize)(obj.privd, width, height);
        
        }
    }

    pub fn set_parent (&self, widget: &WidgetType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_parent)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn set_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).set_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
        }
    }

    pub fn update (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).update)(obj.privd);
        
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
    fn get_paint_device_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}

impl WidgetType for MenuBar {
    fn get_widget_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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

    pub fn set_style_sheet (&self, filename: &str) -> i32 {
        let str_in_filename_1 = CString::new(filename).unwrap();

        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).set_style_sheet)(obj.privd, str_in_filename_1.as_ptr());
          
            ret_val
          
        
        }
    }

    pub fn exec (&self) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).exec)(obj.privd);
        
        }
    }

    pub fn get_files (&self) -> Vec<Url> {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            let ret_val = ((*obj.funcs).get_files)(obj.privd);
          
            if ret_val.count == 0 {
                Vec::new()
            } else {
                let mut data = Vec::with_capacity(ret_val.count as usize);
                let slice = slice::from_raw_parts(ret_val.elements as *const RUUrl, ret_val.count as usize);

                for item in slice {
                    data.push(Url { obj: Some(*item) });
                }

                data
            }
          
        
        }
    }
}

impl Drop for Application {
    fn drop(&mut self) {
       unsafe {
          let obj = self.obj.unwrap();
          ((*obj.funcs).destroy)(obj.privd as *const RUBase)
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
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }
}

impl LayoutType for Layout {
    fn get_layout_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn add_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
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
    fn get_layout_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
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
        
            ((*obj.funcs).add_widget)(obj.privd, widget.get_widget_type_obj() as *const RUBase);
        
        }
    }

    pub fn add_layout (&self, layout: &LayoutType) {
        
        unsafe {
            let obj = self.obj.unwrap();
        
            ((*obj.funcs).add_layout)(obj.privd, layout.get_layout_type_obj() as *const RUBase);
        
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
    fn get_layout_type_obj(&self) -> *const RUBase {
       let obj = self.obj.unwrap();
       obj.privd as *const RUBase
    }
}


#[macro_export]
macro_rules! set_push_button_pressed_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_pressed_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_push_button_released_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_released_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_list_widget_current_row_changed_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, row: i32) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, row);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_current_row_changed_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_list_widget_item_clicked_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, item: *const RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, &ListWidgetItem { obj: Some(*(item as *const rute::ffi_gen::RUListWidgetItem)) });
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_item_clicked_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_list_widget_item_double_clicked_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, item: *const RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, &ListWidgetItem { obj: Some(*(item as *const rute::ffi_gen::RUListWidgetItem)) });
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_item_double_clicked_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_slider_value_changed_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, value: i32) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, value);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_value_changed_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_action_triggered_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_triggered_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_timer_timeout_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_timeout_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_menu_triggered_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, action: *const RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app, &Action { obj: Some(*(action as *const rute::ffi_gen::RUAction)) });
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_triggered_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_application_about_to_quit_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void) {
          unsafe {
              let app = self_c as *mut $call_type;
              $callback(&mut *app);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_about_to_quit_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[macro_export]
macro_rules! set_drag_enter_event {
  ($sender:expr, $data:expr, $call_type:ident, $callback:path) => {
    {
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const ::rute::rute::RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = DragEnterEvent { obj: Some(*(event as *const ::rute::ffi_gen::RUDragEnterEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
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
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const ::rute::rute::RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = DropEvent { obj: Some(*(event as *const ::rute::ffi_gen::RUDropEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
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
      extern "C" fn temp_call(self_c: *const ::std::os::raw::c_void, event: *const ::rute::rute::RUBase) {
          unsafe {
              let app = self_c as *mut $call_type;
              let event = PaintEvent { obj: Some(*(event as *const ::rute::ffi_gen::RUPaintEvent)) };
              $callback(&mut *app, &event);
          }
      }
      fn get_data_ptr(val: &$call_type) -> *const ::std::os::raw::c_void {
         let t: *const ::std::os::raw::c_void = unsafe { ::std::mem::transmute(val) };
         t
      }

      unsafe {
          let obj = $sender.obj.unwrap();
         ((*obj.funcs).set_paint_event)(obj.privd, get_data_ptr($data), temp_call);
      }
    }
} }


#[derive(Copy, Clone, Default)]
pub struct PluginUi {
    pu: Option<*const RUPluginUI>
}

impl PluginUi {
    pub fn new(pu: *const RUPluginUI) -> PluginUi { PluginUi { pu: Some(pu) } }
    pub fn get_c_api(&self) -> *const RUPluginUI { self.pu.unwrap() }

    pub fn create_widget(&self) -> Widget {
        Widget { obj: Some(unsafe { ((*self.pu.unwrap()).create_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_push_button(&self) -> PushButton {
        PushButton { obj: Some(unsafe { ((*self.pu.unwrap()).create_push_button)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_painter(&self) -> Painter {
        Painter { obj: Some(unsafe { ((*self.pu.unwrap()).create_painter)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_list_widget_item(&self) -> ListWidgetItem {
        ListWidgetItem { obj: Some(unsafe { ((*self.pu.unwrap()).create_list_widget_item)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_list_widget(&self) -> ListWidget {
        ListWidget { obj: Some(unsafe { ((*self.pu.unwrap()).create_list_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_label(&self) -> Label {
        Label { obj: Some(unsafe { ((*self.pu.unwrap()).create_label)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_line_edit(&self) -> LineEdit {
        LineEdit { obj: Some(unsafe { ((*self.pu.unwrap()).create_line_edit)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_plain_text_edit(&self) -> PlainTextEdit {
        PlainTextEdit { obj: Some(unsafe { ((*self.pu.unwrap()).create_plain_text_edit)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_slider(&self) -> Slider {
        Slider { obj: Some(unsafe { ((*self.pu.unwrap()).create_slider)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_main_window(&self) -> MainWindow {
        MainWindow { obj: Some(unsafe { ((*self.pu.unwrap()).create_main_window)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_dock_widget(&self) -> DockWidget {
        DockWidget { obj: Some(unsafe { ((*self.pu.unwrap()).create_dock_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_frameless_window(&self) -> FramelessWindow {
        FramelessWindow { obj: Some(unsafe { ((*self.pu.unwrap()).create_frameless_window)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_action(&self) -> Action {
        Action { obj: Some(unsafe { ((*self.pu.unwrap()).create_action)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_timer(&self) -> Timer {
        Timer { obj: Some(unsafe { ((*self.pu.unwrap()).create_timer)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_icon(&self) -> Icon {
        Icon { obj: Some(unsafe { ((*self.pu.unwrap()).create_icon)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_font(&self) -> Font {
        Font { obj: Some(unsafe { ((*self.pu.unwrap()).create_font)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_menu(&self) -> Menu {
        Menu { obj: Some(unsafe { ((*self.pu.unwrap()).create_menu)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_menu_bar(&self) -> MenuBar {
        MenuBar { obj: Some(unsafe { ((*self.pu.unwrap()).create_menu_bar)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_v_box_layout(&self) -> VBoxLayout {
        VBoxLayout { obj: Some(unsafe { ((*self.pu.unwrap()).create_v_box_layout)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_h_box_layout(&self) -> HBoxLayout {
        HBoxLayout { obj: Some(unsafe { ((*self.pu.unwrap()).create_h_box_layout)((*self.pu.unwrap()).privd) }) }
    }

    pub fn get_parent(&self) -> Widget {
        Widget { obj: Some(unsafe { ((*self.pu.unwrap()).get_parent)((*self.pu.unwrap()).privd) }) }
    }

}

#[derive(Copy, Clone, Default)]
pub struct Ui {
    pu: Option<*const RU>
}

impl Ui {
    pub fn new(pu: *const RU) -> Ui { Ui { pu: Some(pu) } }

    pub fn get_c_api(&self) -> *const RU { self.pu.unwrap() }

    pub fn create_widget(&self) -> Widget {
        Widget { obj: Some(unsafe { ((*self.pu.unwrap()).create_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_push_button(&self) -> PushButton {
        PushButton { obj: Some(unsafe { ((*self.pu.unwrap()).create_push_button)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_painter(&self) -> Painter {
        Painter { obj: Some(unsafe { ((*self.pu.unwrap()).create_painter)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_list_widget_item(&self) -> ListWidgetItem {
        ListWidgetItem { obj: Some(unsafe { ((*self.pu.unwrap()).create_list_widget_item)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_list_widget(&self) -> ListWidget {
        ListWidget { obj: Some(unsafe { ((*self.pu.unwrap()).create_list_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_label(&self) -> Label {
        Label { obj: Some(unsafe { ((*self.pu.unwrap()).create_label)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_line_edit(&self) -> LineEdit {
        LineEdit { obj: Some(unsafe { ((*self.pu.unwrap()).create_line_edit)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_plain_text_edit(&self) -> PlainTextEdit {
        PlainTextEdit { obj: Some(unsafe { ((*self.pu.unwrap()).create_plain_text_edit)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_slider(&self) -> Slider {
        Slider { obj: Some(unsafe { ((*self.pu.unwrap()).create_slider)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_main_window(&self) -> MainWindow {
        MainWindow { obj: Some(unsafe { ((*self.pu.unwrap()).create_main_window)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_tool_window_manager(&self) -> ToolWindowManager {
        ToolWindowManager { obj: Some(unsafe { ((*self.pu.unwrap()).create_tool_window_manager)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_dock_widget(&self) -> DockWidget {
        DockWidget { obj: Some(unsafe { ((*self.pu.unwrap()).create_dock_widget)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_dock_manager(&self) -> DockManager {
        DockManager { obj: Some(unsafe { ((*self.pu.unwrap()).create_dock_manager)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_frameless_window(&self) -> FramelessWindow {
        FramelessWindow { obj: Some(unsafe { ((*self.pu.unwrap()).create_frameless_window)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_action(&self) -> Action {
        Action { obj: Some(unsafe { ((*self.pu.unwrap()).create_action)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_timer(&self) -> Timer {
        Timer { obj: Some(unsafe { ((*self.pu.unwrap()).create_timer)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_icon(&self) -> Icon {
        Icon { obj: Some(unsafe { ((*self.pu.unwrap()).create_icon)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_font(&self) -> Font {
        Font { obj: Some(unsafe { ((*self.pu.unwrap()).create_font)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_menu(&self) -> Menu {
        Menu { obj: Some(unsafe { ((*self.pu.unwrap()).create_menu)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_menu_bar(&self) -> MenuBar {
        MenuBar { obj: Some(unsafe { ((*self.pu.unwrap()).create_menu_bar)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_application(&self) -> Application {
        Application { obj: Some(unsafe { ((*self.pu.unwrap()).create_application)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_v_box_layout(&self) -> VBoxLayout {
        VBoxLayout { obj: Some(unsafe { ((*self.pu.unwrap()).create_v_box_layout)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_h_box_layout(&self) -> HBoxLayout {
        HBoxLayout { obj: Some(unsafe { ((*self.pu.unwrap()).create_h_box_layout)((*self.pu.unwrap()).privd) }) }
    }

    pub fn create_plugin_ui(&self, parent: &WidgetType) -> PluginUi {
        PluginUi { pu: Some(unsafe { ((*self.pu.unwrap()).create_plugin_ui)((*self.pu.unwrap()).privd, parent.get_widget_type_obj()) }) }
    }
}


