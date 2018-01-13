use std::os::raw::c_void;

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]
pub struct PUBase {
    _unused: [u8; 0],
}

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub struct PUArray {
    pub priv_data: *const c_void,
    pub elements: *const c_void,
    pub count: i32,
}

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]
pub struct PULayoutType {
    _unused: [u8; 0],
}

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]
pub struct PUPaintDevice {
    _unused: [u8; 0],
}

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]
pub struct PUWidgetType {
    _unused: [u8; 0],
}

#[repr(C)]
pub struct PURect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32,
}

#[repr(C)]
pub struct PUWidgetFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub set_paint_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, event: *const PUBase)),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUWidget {
    pub funcs: *const PUWidgetFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUPushButtonFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub set_released_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub set_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub set_flat: extern "C" fn(self_c: *const PUBase, flat: bool),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUPushButton {
    pub funcs: *const PUPushButtonFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUPainterFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub begin: extern "C" fn(self_c: *const PUBase, target: *const PUBase),
    pub end: extern "C" fn(self_c: *const PUBase),
    pub draw_line: extern "C" fn(self_c: *const PUBase, x1: i32, y1: i32, x2: i32, y2: i32),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUPainter {
    pub funcs: *const PUPainterFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUListWidgetItemFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub set_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub text: extern "C" fn(self_c: *const PUBase) -> *const ::std::os::raw::c_char,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUListWidgetItem {
    pub funcs: *const PUListWidgetItemFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUListWidgetFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub add_item: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub item: extern "C" fn(self_c: *const PUBase, index: i32) ->  PUListWidgetItem,
    pub selected_items: extern "C" fn(self_c: *const PUBase) -> PUArray,
    pub set_drag_enabled: extern "C" fn(self_c: *const PUBase, state: bool),
    pub set_drop_indicator_shown: extern "C" fn(self_c: *const PUBase, state: bool),
    pub set_accept_drops: extern "C" fn(self_c: *const PUBase, state: bool),
    pub add_widget_item: extern "C" fn(self_c: *const PUBase, item: *const PUBase),
    pub set_current_row_changed_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, row: i32)),
    pub set_item_clicked_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, item: *const PUBase)),
    pub set_item_double_clicked_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, item: *const PUBase)),
    pub set_drag_enter_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, event: *const PUBase)),
    pub set_drop_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, event: *const PUBase)),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUListWidget {
    pub funcs: *const PUListWidgetFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUSliderFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub set_value_changed_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, value: i32)),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUSlider {
    pub funcs: *const PUSliderFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUMainWindowFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub is_animated: extern "C" fn(self_c: *const PUBase) -> bool,
    pub menu_bar: extern "C" fn(self_c: *const PUBase) ->  PUMenuBar,
    pub set_central_widget: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUMainWindow {
    pub funcs: *const PUMainWindowFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUFramelessWindowFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub set_window_title: extern "C" fn(self_c: *const PUBase, title: *const ::std::os::raw::c_char),
    pub set_content: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUFramelessWindow {
    pub funcs: *const PUFramelessWindowFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUActionFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub is_enabled: extern "C" fn(self_c: *const PUBase) -> bool,
    pub set_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub set_triggered_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUAction {
    pub funcs: *const PUActionFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUUrlFuncs {
    pub is_local_file: extern "C" fn(self_c: *const PUBase) -> bool,
    pub to_local_file: extern "C" fn(self_c: *const PUBase) -> *const ::std::os::raw::c_char,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUUrl {
    pub funcs: *const PUUrlFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUMimeDataFuncs {
    pub has_color: extern "C" fn(self_c: *const PUBase) -> bool,
    pub has_image: extern "C" fn(self_c: *const PUBase) -> bool,
    pub has_text: extern "C" fn(self_c: *const PUBase) -> bool,
    pub has_urls: extern "C" fn(self_c: *const PUBase) -> bool,
    pub urls: extern "C" fn(self_c: *const PUBase) -> PUArray,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUMimeData {
    pub funcs: *const PUMimeDataFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUMenuFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub add_action_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub add_action: extern "C" fn(self_c: *const PUBase, action: *const PUBase),
    pub set_title: extern "C" fn(self_c: *const PUBase, title: *const ::std::os::raw::c_char),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUMenu {
    pub funcs: *const PUMenuFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUMenuBarFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub add_menu: extern "C" fn(self_c: *const PUBase, menu: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUMenuBar {
    pub funcs: *const PUMenuBarFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUApplicationFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub set_style: extern "C" fn(self_c: *const PUBase, style: *const ::std::os::raw::c_char),
    pub exec: extern "C" fn(self_c: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUApplication {
    pub funcs: *const PUApplicationFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUPaintEventFuncs {
    pub rect: extern "C" fn(self_c: *const PUBase) ->  PURect,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUPaintEvent {
    pub funcs: *const PUPaintEventFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUDragEnterEventFuncs {
    pub accept: extern "C" fn(self_c: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUDragEnterEvent {
    pub funcs: *const PUDragEnterEventFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUDropEventFuncs {
    pub accept_proposed_action: extern "C" fn(self_c: *const PUBase),
    pub mime_data: extern "C" fn(self_c: *const PUBase) ->  PUMimeData,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUDropEvent {
    pub funcs: *const PUDropEventFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PULayoutFuncs {
    pub add_widget: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PULayout {
    pub funcs: *const PULayoutFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUVBoxLayoutFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub add_widget: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUVBoxLayout {
    pub funcs: *const PUVBoxLayoutFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUHBoxLayoutFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub add_widget: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUHBoxLayout {
    pub funcs: *const PUHBoxLayoutFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PU {
    pub create_widget: extern "C" fn(priv_data: *const PUBase) -> PUWidget,
    pub create_push_button: extern "C" fn(priv_data: *const PUBase) -> PUPushButton,
    pub create_painter: extern "C" fn(priv_data: *const PUBase) -> PUPainter,
    pub create_list_widget_item: extern "C" fn(priv_data: *const PUBase) -> PUListWidgetItem,
    pub create_list_widget: extern "C" fn(priv_data: *const PUBase) -> PUListWidget,
    pub create_slider: extern "C" fn(priv_data: *const PUBase) -> PUSlider,
    pub create_main_window: extern "C" fn(priv_data: *const PUBase) -> PUMainWindow,
    pub create_frameless_window: extern "C" fn(priv_data: *const PUBase) -> PUFramelessWindow,
    pub create_action: extern "C" fn(priv_data: *const PUBase) -> PUAction,
    pub create_menu: extern "C" fn(priv_data: *const PUBase) -> PUMenu,
    pub create_menu_bar: extern "C" fn(priv_data: *const PUBase) -> PUMenuBar,
    pub create_application: extern "C" fn(priv_data: *const PUBase) -> PUApplication,
    pub create_v_box_layout: extern "C" fn(priv_data: *const PUBase) -> PUVBoxLayout,
    pub create_h_box_layout: extern "C" fn(priv_data: *const PUBase) -> PUHBoxLayout,
    pub privd: *const PUBase,
}

