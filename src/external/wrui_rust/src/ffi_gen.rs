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
#[derive(Copy, Clone, Debug)]
pub enum PUMetaKeys {
    Ctrl,
}

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub enum PUKeys {
    KeyEscape,
    KeyTab,
    KeyBacktab,
    KeyBackspace,
    KeyReturn,
    KeyEnter,
    KeyInsert,
    KeyDelete,
    KeyPause,
    KeyPrint,
    KeySysReq,
    KeyClear,
    KeyHome,
    KeyEnd,
    KeyLeft,
    KeyUp,
    KeyRight,
    KeyDown,
    KeyPageUp,
    KeyPageDown,
    KeyShift,
    KeyControl,
    KeyMeta,
    KeyAlt,
    KeyCapsLock,
    KeyNumLock,
    KeyScrollLock,
    KeyF1,
    KeyF2,
    KeyF3,
    KeyF4,
    KeyF5,
    KeyF6,
    KeyF7,
    KeyF8,
    KeyF9,
    KeyF10,
    KeyF11,
    KeyF12,
    KeyF13,
    KeyF14,
    KeyF15,
    KeyF16,
    KeyF17,
    KeyF18,
    KeyF19,
    KeyF20,
    KeyF21,
    KeyF22,
    KeyF23,
    KeyF24,
    KeyF25,
    KeyF26,
    KeyF27,
    KeyF28,
    KeyF29,
    KeyF30,
    KeyF31,
    KeyF32,
    KeyF33,
    KeyF34,
    KeyF35,
    KeySuperL,
    KeySuperR,
    KeyMenu,
    KeyHyperL,
    KeyHyperR,
    KeyHelp,
    KeyDirectionL,
    KeyDirectionR,
    KeySpace,
    KeyAny,
    KeyExclam,
    KeyQuoteDbl,
    KeyNumberSign,
    KeyDollar,
    KeyPercent,
    KeyAmpersand,
    KeyApostrophe,
    KeyParenLeft,
    KeyParenRight,
    KeyAsterisk,
    KeyPlus,
    KeyComma,
    KeyMinus,
    KeyPeriod,
    KeySlash,
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    KeyColon,
    KeySemicolon,
    KeyLess,
    KeyEqual,
    KeyGreater,
    KeyQuestion,
    KeyAt,
    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,
    KeyBracketLeft,
    KeyBackslash,
    KeyBracketRight,
    KeyAsciiCircum,
    KeyUnderscore,
    KeyQuoteLeft,
    KeyBraceLeft,
    KeyBar,
    KeyBraceRight,
    KeyAsciiTilde,
    KeyBack,
    KeyForward,
    KeyStop,
    KeyRefresh,
    KeyVolumeDown,
    KeyVolumeMute,
    KeyVolumeUp,
    KeyBassBoost,
    KeyBassUp,
    KeyBassDown,
    KeyTrebleUp,
    KeyTrebleDown,
    KeyMediaPlay,
    KeyMediaStop,
    KeyMediaPrevious,
    KeyMediaNext,
    KeyMediaRecord,
    KeyMediaPause,
    KeyMediaTogglePlayPause,
    KeyHomePage,
    KeyFavorites,
    KeySearch,
    KeyStandby,
    KeyOpenUrl,
    KeyLaunchMail,
    KeyLaunchMedia,
    KeyLaunch0,
    KeyLaunch1,
    KeyLaunch2,
    KeyLaunch3,
    KeyLaunch4,
    KeyLaunch5,
    KeyLaunch6,
    KeyLaunch7,
    KeyLaunch8,
    KeyLaunch9,
    KeyLaunchA,
    KeyLaunchB,
    KeyLaunchC,
    KeyLaunchD,
    KeyLaunchE,
    KeyLaunchF,
    KeyMonBrightnessUp,
    KeyMonBrightnessDown,
    KeyKeyboardLightOnOff,
    KeyKeyboardBrightnessUp,
    KeyKeyboardBrightnessDown,
    KeyPowerOff,
    KeyWakeUp,
    KeyEject,
    KeyScreenSaver,
    KeyWww,
    KeyMemo,
    KeyLightBulb,
    KeyShop,
    KeyHistory,
    KeyAddFavorite,
    KeyHotLinks,
    KeyBrightnessAdjust,
    KeyFinance,
    KeyCommunity,
    KeyAudioRewind,
    KeyBackForward,
    KeyApplicationLeft,
    KeyApplicationRight,
    KeyBook,
    KeyCd,
    KeyCalculator,
    KeyToDoList,
    KeyClearGrab,
    KeyClose,
    KeyCopy,
    KeyCut,
    KeyDisplay,
    KeyDos,
    KeyDocuments,
    KeyExcel,
    KeyExplorer,
    KeyGame,
    KeyGo,
    KeyITouch,
    KeyLogOff,
    KeyMarket,
    KeyMeeting,
    KeyMenuKb,
    KeyMenuPb,
    KeyMySites,
    KeyNews,
    KeyOfficeHome,
    KeyOption,
    KeyPaste,
    KeyPhone,
    KeyCalendar,
    KeyReply,
    KeyReload,
    KeyRotateWindows,
    KeyRotationPb,
    KeyRotationKb,
    KeySave,
    KeySend,
    KeySpell,
    KeySplitScreen,
    KeySupport,
    KeyTaskPane,
    KeyTerminal,
    KeyTools,
    KeyTravel,
    KeyVideo,
    KeyWord,
    KeyXfer,
    KeyZoomIn,
    KeyZoomOut,
    KeyAway,
    KeyMessenger,
    KeyWebCam,
    KeyMailForward,
    KeyPictures,
    KeyMusic,
    KeyBattery,
    KeyBluetooth,
    KeyWlan,
    KeyUwb,
    KeyAudioForward,
    KeyAudioRepeat,
    KeyAudioRandomPlay,
    KeySubtitle,
    KeyAudioCycleTrack,
    KeyTime,
    KeyHibernate,
    KeyView,
    KeyTopMenu,
    KeyPowerDown,
    KeySuspend,
    KeyContrastAdjust,
    KeyLaunchG,
    KeyLaunchH,
    KeyTouchpadToggle,
    KeyTouchpadOn,
    KeyTouchpadOff,
    KeyMicMute,
    KeyRed,
    KeyGreen,
    KeyYellow,
    KeyBlue,
    KeyChannelUp,
    KeyChannelDown,
    KeyGuide,
    KeyInfo,
    KeySettings,
    KeyMicVolumeUp,
    KeyMicVolumeDown,
    KeyNew,
    KeyOpen,
    KeyFind,
    KeyUndo,
    KeyRedo,
    KeyMediaLast,
    KeySelect,
    KeyYes,
    KeyNo,
    KeyCancel,
    KeyPrinter,
    KeyExecute,
    KeySleep,
    KeyPlay,
    KeyZoom,
    KeyExit,
    KeyContext1,
    KeyContext2,
    KeyContext3,
    KeyContext4,
    KeyCall,
    KeyHangup,
    KeyFlip,
    KeyToggleCallHangup,
    KeyVoiceDial,
    KeyLastNumberRedial,
    KeyCamera,
    KeyCameraFocus,
}

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]pub struct PURect {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub height: i32,
}

#[repr(C)]
#[derive(Default, Copy, Clone, Debug)]pub struct PUColor {
    pub r: u16,
    pub g: u16,
    pub b: u16,
    pub a: u16,
}

#[repr(C)]
pub struct PUWidgetFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
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
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub set_pressed_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub set_released_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub set_icon: extern "C" fn(self_c: *const PUBase, icon: *const PUBase),
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
    pub set_font: extern "C" fn(self_c: *const PUBase, font: *const PUBase),
    pub draw_text: extern "C" fn(self_c: *const PUBase, x: i32, y: i32, text: *const ::std::os::raw::c_char),
    pub draw_line: extern "C" fn(self_c: *const PUBase, x1: i32, y1: i32, x2: i32, y2: i32),
    pub fill_rect_color: extern "C" fn(self_c: *const PUBase, rect:  PURect, color:  PUColor),
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
    pub set_string_data: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub get_string_data: extern "C" fn(self_c: *const PUBase) -> *const ::std::os::raw::c_char,
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
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub add_item: extern "C" fn(self_c: *const PUBase, item: *const PUBase),
    pub add_text_item: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub current_item: extern "C" fn(self_c: *const PUBase) ->  PUListWidgetItem,
    pub current_row: extern "C" fn(self_c: *const PUBase) -> i32,
    pub selected_items: extern "C" fn(self_c: *const PUBase) -> PUArray,
    pub item: extern "C" fn(self_c: *const PUBase, index: i32) ->  PUListWidgetItem,
    pub set_current_row: extern "C" fn(self_c: *const PUBase, index: i32),
    pub count: extern "C" fn(self_c: *const PUBase) -> i32,
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
pub struct PULabelFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub set_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PULabel {
    pub funcs: *const PULabelFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PULineEditFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub set_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub set_read_only: extern "C" fn(self_c: *const PUBase, value: bool),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PULineEdit {
    pub funcs: *const PULineEditFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUPlainTextEditFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub clear: extern "C" fn(self_c: *const PUBase),
    pub set_plain_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub append_plain_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub set_read_only: extern "C" fn(self_c: *const PUBase, value: bool),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUPlainTextEdit {
    pub funcs: *const PUPlainTextEditFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUSliderFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
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
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
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
pub struct PUToolWindowManagerFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub add_to_docking: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUToolWindowManager {
    pub funcs: *const PUToolWindowManagerFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUFramelessWindowFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
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
    pub set_shortcut: extern "C" fn(self_c: *const PUBase, key:  PUKeys),
    pub set_shortcut_mod: extern "C" fn(self_c: *const PUBase, key:  PUKeys, modifier:  PUMetaKeys),
    pub set_triggered_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub set_int_data: extern "C" fn(self_c: *const PUBase, data: i32),
    pub get_int_data: extern "C" fn(self_c: *const PUBase) -> i32,
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
pub struct PUTimerFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub set_timeout_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub start: extern "C" fn(self_c: *const PUBase, time: i32),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUTimer {
    pub funcs: *const PUTimerFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUIconFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub add_file: extern "C" fn(self_c: *const PUBase, filename: *const ::std::os::raw::c_char),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUIcon {
    pub funcs: *const PUIconFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUFontFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub set_family: extern "C" fn(self_c: *const PUBase, family: *const ::std::os::raw::c_char),
    pub set_point_size: extern "C" fn(self_c: *const PUBase, size: i32),
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct PUFont {
    pub funcs: *const PUFontFuncs,
    pub privd: *const PUBase,
}

#[repr(C)]
pub struct PUMenuFuncs {
    pub destroy: extern "C" fn(self_c: *const PUBase),
    pub show: extern "C" fn(self_c: *const PUBase),
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
    pub add_action_text: extern "C" fn(self_c: *const PUBase, text: *const ::std::os::raw::c_char),
    pub set_triggered_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void, action: *const PUBase)),
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
    pub set_fixed_height: extern "C" fn(self_c: *const PUBase, width: i32),
    pub set_fixed_width: extern "C" fn(self_c: *const PUBase, width: i32),
    pub resize: extern "C" fn(self_c: *const PUBase, width: i32, height: i32),
    pub set_parent: extern "C" fn(self_c: *const PUBase, widget: *const PUBase),
    pub set_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
    pub update: extern "C" fn(self_c: *const PUBase),
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
    pub set_style_sheet: extern "C" fn(self_c: *const PUBase, filename: *const ::std::os::raw::c_char) -> i32,
    pub exec: extern "C" fn(self_c: *const PUBase),
    pub set_about_to_quit_event: extern "C" fn(object: *const PUBase, user_data: *const c_void,
                                        callback: extern "C" fn(self_c: *const c_void)),
    pub get_files: extern "C" fn(self_c: *const PUBase) -> PUArray,
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
    pub add_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
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
    pub add_layout: extern "C" fn(self_c: *const PUBase, layout: *const PUBase),
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
    pub create_label: extern "C" fn(priv_data: *const PUBase) -> PULabel,
    pub create_line_edit: extern "C" fn(priv_data: *const PUBase) -> PULineEdit,
    pub create_plain_text_edit: extern "C" fn(priv_data: *const PUBase) -> PUPlainTextEdit,
    pub create_slider: extern "C" fn(priv_data: *const PUBase) -> PUSlider,
    pub create_main_window: extern "C" fn(priv_data: *const PUBase) -> PUMainWindow,
    pub create_tool_window_manager: extern "C" fn(priv_data: *const PUBase) -> PUToolWindowManager,
    pub create_frameless_window: extern "C" fn(priv_data: *const PUBase) -> PUFramelessWindow,
    pub create_action: extern "C" fn(priv_data: *const PUBase) -> PUAction,
    pub create_timer: extern "C" fn(priv_data: *const PUBase) -> PUTimer,
    pub create_icon: extern "C" fn(priv_data: *const PUBase) -> PUIcon,
    pub create_font: extern "C" fn(priv_data: *const PUBase) -> PUFont,
    pub create_menu: extern "C" fn(priv_data: *const PUBase) -> PUMenu,
    pub create_menu_bar: extern "C" fn(priv_data: *const PUBase) -> PUMenuBar,
    pub create_application: extern "C" fn(priv_data: *const PUBase) -> PUApplication,
    pub create_v_box_layout: extern "C" fn(priv_data: *const PUBase) -> PUVBoxLayout,
    pub create_h_box_layout: extern "C" fn(priv_data: *const PUBase) -> PUHBoxLayout,
    pub open_files_dialog: extern "C" fn(self_c: *const PUBase) -> PUArray,
    pub privd: *const PUBase,
}

