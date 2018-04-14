
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct PUBase;

struct PUArray {
    void* priv_data;
    void* elements;
    uint32_t count;
};

struct PURect;
struct PUColor;
struct PUWidget;
struct PUWidgetFuncs;
struct PUPushButton;
struct PUPushButtonFuncs;
struct PUPainter;
struct PUPainterFuncs;
struct PUListWidgetItem;
struct PUListWidgetItemFuncs;
struct PUListWidget;
struct PUListWidgetFuncs;
struct PULabel;
struct PULabelFuncs;
struct PULineEdit;
struct PULineEditFuncs;
struct PUPlainTextEdit;
struct PUPlainTextEditFuncs;
struct PUSlider;
struct PUSliderFuncs;
struct PUMainWindow;
struct PUMainWindowFuncs;
struct PUFramelessWindow;
struct PUFramelessWindowFuncs;
struct PUAction;
struct PUActionFuncs;
struct PUUrl;
struct PUUrlFuncs;
struct PUMimeData;
struct PUMimeDataFuncs;
struct PUTimer;
struct PUTimerFuncs;
struct PUIcon;
struct PUIconFuncs;
struct PUFont;
struct PUFontFuncs;
struct PUMenu;
struct PUMenuFuncs;
struct PUMenuBar;
struct PUMenuBarFuncs;
struct PUApplication;
struct PUApplicationFuncs;
struct PUPaintEvent;
struct PUPaintEventFuncs;
struct PUDragEnterEvent;
struct PUDragEnterEventFuncs;
struct PUDropEvent;
struct PUDropEventFuncs;
struct PULayout;
struct PULayoutFuncs;
struct PUVBoxLayout;
struct PUVBoxLayoutFuncs;
struct PUHBoxLayout;
struct PUHBoxLayoutFuncs;
struct PULayoutType;
struct PUPaintDevice;
struct PUWidgetType;

typedef enum PUMetaKeys {
    PUMetaKeys_CTRL,
} PUMetaKeys;

typedef enum PUKeys {
    PUKeys_Key_Escape,
    PUKeys_Key_Tab,
    PUKeys_Key_Backtab,
    PUKeys_Key_Backspace,
    PUKeys_Key_Return,
    PUKeys_Key_Enter,
    PUKeys_Key_Insert,
    PUKeys_Key_Delete,
    PUKeys_Key_Pause,
    PUKeys_Key_Print,
    PUKeys_Key_SysReq,
    PUKeys_Key_Clear,
    PUKeys_Key_Home,
    PUKeys_Key_End,
    PUKeys_Key_Left,
    PUKeys_Key_Up,
    PUKeys_Key_Right,
    PUKeys_Key_Down,
    PUKeys_Key_PageUp,
    PUKeys_Key_PageDown,
    PUKeys_Key_Shift,
    PUKeys_Key_Control,
    PUKeys_Key_Meta,
    PUKeys_Key_Alt,
    PUKeys_Key_CapsLock,
    PUKeys_Key_NumLock,
    PUKeys_Key_ScrollLock,
    PUKeys_Key_F1,
    PUKeys_Key_F2,
    PUKeys_Key_F3,
    PUKeys_Key_F4,
    PUKeys_Key_F5,
    PUKeys_Key_F6,
    PUKeys_Key_F7,
    PUKeys_Key_F8,
    PUKeys_Key_F9,
    PUKeys_Key_F10,
    PUKeys_Key_F11,
    PUKeys_Key_F12,
    PUKeys_Key_F13,
    PUKeys_Key_F14,
    PUKeys_Key_F15,
    PUKeys_Key_F16,
    PUKeys_Key_F17,
    PUKeys_Key_F18,
    PUKeys_Key_F19,
    PUKeys_Key_F20,
    PUKeys_Key_F21,
    PUKeys_Key_F22,
    PUKeys_Key_F23,
    PUKeys_Key_F24,
    PUKeys_Key_F25,
    PUKeys_Key_F26,
    PUKeys_Key_F27,
    PUKeys_Key_F28,
    PUKeys_Key_F29,
    PUKeys_Key_F30,
    PUKeys_Key_F31,
    PUKeys_Key_F32,
    PUKeys_Key_F33,
    PUKeys_Key_F34,
    PUKeys_Key_F35,
    PUKeys_Key_Super_L,
    PUKeys_Key_Super_R,
    PUKeys_Key_Menu,
    PUKeys_Key_Hyper_L,
    PUKeys_Key_Hyper_R,
    PUKeys_Key_Help,
    PUKeys_Key_Direction_L,
    PUKeys_Key_Direction_R,
    PUKeys_Key_Space,
    PUKeys_Key_Any,
    PUKeys_Key_Exclam,
    PUKeys_Key_QuoteDbl,
    PUKeys_Key_NumberSign,
    PUKeys_Key_Dollar,
    PUKeys_Key_Percent,
    PUKeys_Key_Ampersand,
    PUKeys_Key_Apostrophe,
    PUKeys_Key_ParenLeft,
    PUKeys_Key_ParenRight,
    PUKeys_Key_Asterisk,
    PUKeys_Key_Plus,
    PUKeys_Key_Comma,
    PUKeys_Key_Minus,
    PUKeys_Key_Period,
    PUKeys_Key_Slash,
    PUKeys_Key_0,
    PUKeys_Key_1,
    PUKeys_Key_2,
    PUKeys_Key_3,
    PUKeys_Key_4,
    PUKeys_Key_5,
    PUKeys_Key_6,
    PUKeys_Key_7,
    PUKeys_Key_8,
    PUKeys_Key_9,
    PUKeys_Key_Colon,
    PUKeys_Key_Semicolon,
    PUKeys_Key_Less,
    PUKeys_Key_Equal,
    PUKeys_Key_Greater,
    PUKeys_Key_Question,
    PUKeys_Key_At,
    PUKeys_Key_A,
    PUKeys_Key_B,
    PUKeys_Key_C,
    PUKeys_Key_D,
    PUKeys_Key_E,
    PUKeys_Key_F,
    PUKeys_Key_G,
    PUKeys_Key_H,
    PUKeys_Key_I,
    PUKeys_Key_J,
    PUKeys_Key_K,
    PUKeys_Key_L,
    PUKeys_Key_M,
    PUKeys_Key_N,
    PUKeys_Key_O,
    PUKeys_Key_P,
    PUKeys_Key_Q,
    PUKeys_Key_R,
    PUKeys_Key_S,
    PUKeys_Key_T,
    PUKeys_Key_U,
    PUKeys_Key_V,
    PUKeys_Key_W,
    PUKeys_Key_X,
    PUKeys_Key_Y,
    PUKeys_Key_Z,
    PUKeys_Key_BracketLeft,
    PUKeys_Key_Backslash,
    PUKeys_Key_BracketRight,
    PUKeys_Key_AsciiCircum,
    PUKeys_Key_Underscore,
    PUKeys_Key_QuoteLeft,
    PUKeys_Key_BraceLeft,
    PUKeys_Key_Bar,
    PUKeys_Key_BraceRight,
    PUKeys_Key_AsciiTilde,
    PUKeys_Key_Back,
    PUKeys_Key_Forward,
    PUKeys_Key_Stop,
    PUKeys_Key_Refresh,
    PUKeys_Key_VolumeDown,
    PUKeys_Key_VolumeMute,
    PUKeys_Key_VolumeUp,
    PUKeys_Key_BassBoost,
    PUKeys_Key_BassUp,
    PUKeys_Key_BassDown,
    PUKeys_Key_TrebleUp,
    PUKeys_Key_TrebleDown,
    PUKeys_Key_MediaPlay,
    PUKeys_Key_MediaStop,
    PUKeys_Key_MediaPrevious,
    PUKeys_Key_MediaNext,
    PUKeys_Key_MediaRecord,
    PUKeys_Key_MediaPause,
    PUKeys_Key_MediaTogglePlayPause,
    PUKeys_Key_HomePage,
    PUKeys_Key_Favorites,
    PUKeys_Key_Search,
    PUKeys_Key_Standby,
    PUKeys_Key_OpenUrl,
    PUKeys_Key_LaunchMail,
    PUKeys_Key_LaunchMedia,
    PUKeys_Key_Launch0,
    PUKeys_Key_Launch1,
    PUKeys_Key_Launch2,
    PUKeys_Key_Launch3,
    PUKeys_Key_Launch4,
    PUKeys_Key_Launch5,
    PUKeys_Key_Launch6,
    PUKeys_Key_Launch7,
    PUKeys_Key_Launch8,
    PUKeys_Key_Launch9,
    PUKeys_Key_LaunchA,
    PUKeys_Key_LaunchB,
    PUKeys_Key_LaunchC,
    PUKeys_Key_LaunchD,
    PUKeys_Key_LaunchE,
    PUKeys_Key_LaunchF,
    PUKeys_Key_MonBrightnessUp,
    PUKeys_Key_MonBrightnessDown,
    PUKeys_Key_KeyboardLightOnOff,
    PUKeys_Key_KeyboardBrightnessUp,
    PUKeys_Key_KeyboardBrightnessDown,
    PUKeys_Key_PowerOff,
    PUKeys_Key_WakeUp,
    PUKeys_Key_Eject,
    PUKeys_Key_ScreenSaver,
    PUKeys_Key_WWW,
    PUKeys_Key_Memo,
    PUKeys_Key_LightBulb,
    PUKeys_Key_Shop,
    PUKeys_Key_History,
    PUKeys_Key_AddFavorite,
    PUKeys_Key_HotLinks,
    PUKeys_Key_BrightnessAdjust,
    PUKeys_Key_Finance,
    PUKeys_Key_Community,
    PUKeys_Key_AudioRewind,
    PUKeys_Key_BackForward,
    PUKeys_Key_ApplicationLeft,
    PUKeys_Key_ApplicationRight,
    PUKeys_Key_Book,
    PUKeys_Key_CD,
    PUKeys_Key_Calculator,
    PUKeys_Key_ToDoList,
    PUKeys_Key_ClearGrab,
    PUKeys_Key_Close,
    PUKeys_Key_Copy,
    PUKeys_Key_Cut,
    PUKeys_Key_Display,
    PUKeys_Key_DOS,
    PUKeys_Key_Documents,
    PUKeys_Key_Excel,
    PUKeys_Key_Explorer,
    PUKeys_Key_Game,
    PUKeys_Key_Go,
    PUKeys_Key_iTouch,
    PUKeys_Key_LogOff,
    PUKeys_Key_Market,
    PUKeys_Key_Meeting,
    PUKeys_Key_MenuKB,
    PUKeys_Key_MenuPB,
    PUKeys_Key_MySites,
    PUKeys_Key_News,
    PUKeys_Key_OfficeHome,
    PUKeys_Key_Option,
    PUKeys_Key_Paste,
    PUKeys_Key_Phone,
    PUKeys_Key_Calendar,
    PUKeys_Key_Reply,
    PUKeys_Key_Reload,
    PUKeys_Key_RotateWindows,
    PUKeys_Key_RotationPB,
    PUKeys_Key_RotationKB,
    PUKeys_Key_Save,
    PUKeys_Key_Send,
    PUKeys_Key_Spell,
    PUKeys_Key_SplitScreen,
    PUKeys_Key_Support,
    PUKeys_Key_TaskPane,
    PUKeys_Key_Terminal,
    PUKeys_Key_Tools,
    PUKeys_Key_Travel,
    PUKeys_Key_Video,
    PUKeys_Key_Word,
    PUKeys_Key_Xfer,
    PUKeys_Key_ZoomIn,
    PUKeys_Key_ZoomOut,
    PUKeys_Key_Away,
    PUKeys_Key_Messenger,
    PUKeys_Key_WebCam,
    PUKeys_Key_MailForward,
    PUKeys_Key_Pictures,
    PUKeys_Key_Music,
    PUKeys_Key_Battery,
    PUKeys_Key_Bluetooth,
    PUKeys_Key_WLAN,
    PUKeys_Key_UWB,
    PUKeys_Key_AudioForward,
    PUKeys_Key_AudioRepeat,
    PUKeys_Key_AudioRandomPlay,
    PUKeys_Key_Subtitle,
    PUKeys_Key_AudioCycleTrack,
    PUKeys_Key_Time,
    PUKeys_Key_Hibernate,
    PUKeys_Key_View,
    PUKeys_Key_TopMenu,
    PUKeys_Key_PowerDown,
    PUKeys_Key_Suspend,
    PUKeys_Key_ContrastAdjust,
    PUKeys_Key_LaunchG,
    PUKeys_Key_LaunchH,
    PUKeys_Key_TouchpadToggle,
    PUKeys_Key_TouchpadOn,
    PUKeys_Key_TouchpadOff,
    PUKeys_Key_MicMute,
    PUKeys_Key_Red,
    PUKeys_Key_Green,
    PUKeys_Key_Yellow,
    PUKeys_Key_Blue,
    PUKeys_Key_ChannelUp,
    PUKeys_Key_ChannelDown,
    PUKeys_Key_Guide,
    PUKeys_Key_Info,
    PUKeys_Key_Settings,
    PUKeys_Key_MicVolumeUp,
    PUKeys_Key_MicVolumeDown,
    PUKeys_Key_New,
    PUKeys_Key_Open,
    PUKeys_Key_Find,
    PUKeys_Key_Undo,
    PUKeys_Key_Redo,
    PUKeys_Key_MediaLast,
    PUKeys_Key_Select,
    PUKeys_Key_Yes,
    PUKeys_Key_No,
    PUKeys_Key_Cancel,
    PUKeys_Key_Printer,
    PUKeys_Key_Execute,
    PUKeys_Key_Sleep,
    PUKeys_Key_Play,
    PUKeys_Key_Zoom,
    PUKeys_Key_Exit,
    PUKeys_Key_Context1,
    PUKeys_Key_Context2,
    PUKeys_Key_Context3,
    PUKeys_Key_Context4,
    PUKeys_Key_Call,
    PUKeys_Key_Hangup,
    PUKeys_Key_Flip,
    PUKeys_Key_ToggleCallHangup,
    PUKeys_Key_VoiceDial,
    PUKeys_Key_LastNumberRedial,
    PUKeys_Key_Camera,
    PUKeys_Key_CameraFocus,
} PUKeys;

struct PURect {
    int x;
    int y;
    int width;
    int height;
};

struct PUColor {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t a;
};

struct PUWidgetFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_paint_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event));
};

struct PUWidget {
    struct PUWidgetFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUPushButtonFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_pressed_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_released_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_icon)(struct PUBase* self_c, struct PUBase* icon);
    void (*set_text)(struct PUBase* self_c, const char* text);
    void (*set_flat)(struct PUBase* self_c, bool flat);
};

struct PUPushButton {
    struct PUPushButtonFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUPainterFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*begin)(struct PUBase* self_c, struct PUBase* target);
    void (*end)(struct PUBase* self_c);
    void (*set_font)(struct PUBase* self_c, struct PUBase* font);
    void (*draw_text)(struct PUBase* self_c, int x, int y, const char* text);
    void (*draw_line)(struct PUBase* self_c, int x1, int y1, int x2, int y2);
    void (*fill_rect_color)(struct PUBase* self_c, struct PURect rect, struct PUColor color);
};

struct PUPainter {
    struct PUPainterFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUListWidgetItemFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*set_text)(struct PUBase* self_c, const char* text);
    const char* (*text)(struct PUBase* self_c);
    void (*set_string_data)(struct PUBase* self_c, const char* text);
    const char* (*get_string_data)(struct PUBase* self_c);
};

struct PUListWidgetItem {
    struct PUListWidgetItemFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUListWidgetFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*add_item)(struct PUBase* self_c, struct PUBase* item);
    void (*add_text_item)(struct PUBase* self_c, const char* text);
    struct PUListWidgetItem (*current_item)(struct PUBase* self_c);
    int (*current_row)(struct PUBase* self_c);
    struct PUArray (*selected_items)(struct PUBase* self_c);
    struct PUListWidgetItem (*item)(struct PUBase* self_c, int index);
    void (*set_current_row)(struct PUBase* self_c, int index);
    int (*count)(struct PUBase* self_c);
    void (*set_drag_enabled)(struct PUBase* self_c, bool state);
    void (*set_drop_indicator_shown)(struct PUBase* self_c, bool state);
    void (*set_accept_drops)(struct PUBase* self_c, bool state);
    void (*add_widget_item)(struct PUBase* self_c, struct PUBase* item);
    void (*set_current_row_changed_event)(void* object, void* user_data, void (*event)(void* self_c, int row));
    void (*set_item_clicked_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* item));
    void (*set_item_double_clicked_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* item));
    void (*set_drag_enter_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event));
    void (*set_drop_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event));
};

struct PUListWidget {
    struct PUListWidgetFuncs* funcs;
    struct PUBase* priv_data;
};

struct PULabelFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_text)(struct PUBase* self_c, const char* text);
};

struct PULabel {
    struct PULabelFuncs* funcs;
    struct PUBase* priv_data;
};

struct PULineEditFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_text)(struct PUBase* self_c, const char* text);
    void (*set_read_only)(struct PUBase* self_c, bool value);
};

struct PULineEdit {
    struct PULineEditFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUPlainTextEditFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*clear)(struct PUBase* self_c);
    void (*set_plain_text)(struct PUBase* self_c, const char* text);
    void (*append_plain_text)(struct PUBase* self_c, const char* text);
    void (*set_read_only)(struct PUBase* self_c, bool value);
};

struct PUPlainTextEdit {
    struct PUPlainTextEditFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUSliderFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_value_changed_event)(void* object, void* user_data, void (*event)(void* self_c, int value));
};

struct PUSlider {
    struct PUSliderFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUMainWindowFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    bool (*is_animated)(struct PUBase* self_c);
    struct PUMenuBar (*menu_bar)(struct PUBase* self_c);
    void (*set_central_widget)(struct PUBase* self_c, struct PUBase* widget);
};

struct PUMainWindow {
    struct PUMainWindowFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUFramelessWindowFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*set_window_title)(struct PUBase* self_c, const char* title);
    void (*set_content)(struct PUBase* self_c, struct PUBase* widget);
};

struct PUFramelessWindow {
    struct PUFramelessWindowFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUActionFuncs {
    void (*destroy)(struct PUBase* self_c);
    bool (*is_enabled)(struct PUBase* self_c);
    void (*set_text)(struct PUBase* self_c, const char* text);
    void (*set_shortcut)(struct PUBase* self_c, PUKeys key);
    void (*set_shortcut_mod)(struct PUBase* self_c, PUKeys key, PUMetaKeys modifier);
    void (*set_triggered_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_int_data)(struct PUBase* self_c, int data);
    int (*get_int_data)(struct PUBase* self_c);
};

struct PUAction {
    struct PUActionFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUUrlFuncs {
    bool (*is_local_file)(struct PUBase* self_c);
    const char* (*to_local_file)(struct PUBase* self_c);
};

struct PUUrl {
    struct PUUrlFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUMimeDataFuncs {
    bool (*has_color)(struct PUBase* self_c);
    bool (*has_image)(struct PUBase* self_c);
    bool (*has_text)(struct PUBase* self_c);
    bool (*has_urls)(struct PUBase* self_c);
    struct PUArray (*urls)(struct PUBase* self_c);
};

struct PUMimeData {
    struct PUMimeDataFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUTimerFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*set_timeout_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*start)(struct PUBase* self_c, int time);
};

struct PUTimer {
    struct PUTimerFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUIconFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*add_file)(struct PUBase* self_c, const char* filename);
};

struct PUIcon {
    struct PUIconFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUFontFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*set_family)(struct PUBase* self_c, const char* family);
    void (*set_point_size)(struct PUBase* self_c, int size);
};

struct PUFont {
    struct PUFontFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUMenuFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*add_action_text)(struct PUBase* self_c, const char* text);
    void (*set_triggered_event)(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* action));
    void (*add_action)(struct PUBase* self_c, struct PUBase* action);
    void (*set_title)(struct PUBase* self_c, const char* title);
};

struct PUMenu {
    struct PUMenuFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUMenuBarFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*show)(struct PUBase* self_c);
    void (*set_fixed_height)(struct PUBase* self_c, int width);
    void (*set_fixed_width)(struct PUBase* self_c, int width);
    void (*resize)(struct PUBase* self_c, int width, int height);
    void (*set_parent)(struct PUBase* self_c, struct PUBase* widget);
    void (*set_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
    void (*add_menu)(struct PUBase* self_c, struct PUBase* menu);
};

struct PUMenuBar {
    struct PUMenuBarFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUApplicationFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*set_style)(struct PUBase* self_c, const char* style);
    int (*set_style_sheet)(struct PUBase* self_c, const char* filename);
    void (*exec)(struct PUBase* self_c);
    void (*set_about_to_quit_event)(void* object, void* user_data, void (*event)(void* self_c));
    struct PUArray (*get_files)(struct PUBase* self_c);
};

struct PUApplication {
    struct PUApplicationFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUPaintEventFuncs {
    struct PURect (*rect)(struct PUBase* self_c);
};

struct PUPaintEvent {
    struct PUPaintEventFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUDragEnterEventFuncs {
    void (*accept)(struct PUBase* self_c);
};

struct PUDragEnterEvent {
    struct PUDragEnterEventFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUDropEventFuncs {
    void (*accept_proposed_action)(struct PUBase* self_c);
    struct PUMimeData (*mime_data)(struct PUBase* self_c);
};

struct PUDropEvent {
    struct PUDropEventFuncs* funcs;
    struct PUBase* priv_data;
};

struct PULayoutFuncs {
    void (*add_widget)(struct PUBase* self_c, struct PUBase* widget);
};

struct PULayout {
    struct PULayoutFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUVBoxLayoutFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*add_widget)(struct PUBase* self_c, struct PUBase* widget);
    void (*add_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
};

struct PUVBoxLayout {
    struct PUVBoxLayoutFuncs* funcs;
    struct PUBase* priv_data;
};

struct PUHBoxLayoutFuncs {
    void (*destroy)(struct PUBase* self_c);
    void (*add_widget)(struct PUBase* self_c, struct PUBase* widget);
    void (*add_layout)(struct PUBase* self_c, struct PUBase* layout);
    void (*update)(struct PUBase* self_c);
};

struct PUHBoxLayout {
    struct PUHBoxLayoutFuncs* funcs;
    struct PUBase* priv_data;
};

typedef struct PU { 
    struct PUWidget (*create_widget)(struct PUBase* self);
    struct PUPushButton (*create_push_button)(struct PUBase* self);
    struct PUPainter (*create_painter)(struct PUBase* self);
    struct PUListWidgetItem (*create_list_widget_item)(struct PUBase* self);
    struct PUListWidget (*create_list_widget)(struct PUBase* self);
    struct PULabel (*create_label)(struct PUBase* self);
    struct PULineEdit (*create_line_edit)(struct PUBase* self);
    struct PUPlainTextEdit (*create_plain_text_edit)(struct PUBase* self);
    struct PUSlider (*create_slider)(struct PUBase* self);
    struct PUMainWindow (*create_main_window)(struct PUBase* self);
    struct PUFramelessWindow (*create_frameless_window)(struct PUBase* self);
    struct PUAction (*create_action)(struct PUBase* self);
    struct PUTimer (*create_timer)(struct PUBase* self);
    struct PUIcon (*create_icon)(struct PUBase* self);
    struct PUFont (*create_font)(struct PUBase* self);
    struct PUMenu (*create_menu)(struct PUBase* self);
    struct PUMenuBar (*create_menu_bar)(struct PUBase* self);
    struct PUApplication (*create_application)(struct PUBase* self);
    struct PUVBoxLayout (*create_v_box_layout)(struct PUBase* self);
    struct PUHBoxLayout (*create_h_box_layout)(struct PUBase* self);
    struct PUArray (*open_files_dialog)(struct PUBase* self_c);
    struct PUBase* priv_data;
} PU;

#define PUWidget_show(obj) obj.funcs->show(obj.priv_data)
#define PUWidget_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUWidget_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUWidget_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUWidget_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUWidget_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUWidget_update(obj) obj.funcs->update(obj.priv_data)

#define PUPushButton_show(obj) obj.funcs->show(obj.priv_data)
#define PUPushButton_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUPushButton_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUPushButton_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUPushButton_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUPushButton_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUPushButton_update(obj) obj.funcs->update(obj.priv_data)
#define PUPushButton_set_pressed_event(obj, user_data, event) obj.funcs->set_pressed_event(obj.priv_data, user_data, event)
#define PUPushButton_set_released_event(obj, user_data, event) obj.funcs->set_released_event(obj.priv_data, user_data, event)
#define PUPushButton_set_icon(obj, icon) obj.funcs->set_icon(obj.priv_data, icon)
#define PUPushButton_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define PUPushButton_set_flat(obj, flat) obj.funcs->set_flat(obj.priv_data, flat)

#define PUPainter_begin(obj, target) obj.funcs->begin(obj.priv_data, target)
#define PUPainter_end(obj) obj.funcs->end(obj.priv_data)
#define PUPainter_set_font(obj, font) obj.funcs->set_font(obj.priv_data, font)
#define PUPainter_draw_text(obj, x, y, text) obj.funcs->draw_text(obj.priv_data, x, y, text)
#define PUPainter_draw_line(obj, x1, y1, x2, y2) obj.funcs->draw_line(obj.priv_data, x1, y1, x2, y2)
#define PUPainter_fill_rect_color(obj, rect, color) obj.funcs->fill_rect_color(obj.priv_data, rect, color)

#define PUListWidgetItem_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define PUListWidgetItem_text(obj) obj.funcs->text(obj.priv_data)
#define PUListWidgetItem_set_string_data(obj, text) obj.funcs->set_string_data(obj.priv_data, text)
#define PUListWidgetItem_get_string_data(obj) obj.funcs->get_string_data(obj.priv_data)

#define PUListWidget_show(obj) obj.funcs->show(obj.priv_data)
#define PUListWidget_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUListWidget_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUListWidget_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUListWidget_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUListWidget_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUListWidget_update(obj) obj.funcs->update(obj.priv_data)
#define PUListWidget_add_item(obj, item) obj.funcs->add_item(obj.priv_data, item)
#define PUListWidget_add_text_item(obj, text) obj.funcs->add_text_item(obj.priv_data, text)
#define PUListWidget_current_item(obj) obj.funcs->current_item(obj.priv_data)
#define PUListWidget_current_row(obj) obj.funcs->current_row(obj.priv_data)
#define PUListWidget_selected_items(obj) obj.funcs->selected_items(obj.priv_data)
#define PUListWidget_item(obj, index) obj.funcs->item(obj.priv_data, index)
#define PUListWidget_set_current_row(obj, index) obj.funcs->set_current_row(obj.priv_data, index)
#define PUListWidget_count(obj) obj.funcs->count(obj.priv_data)
#define PUListWidget_set_drag_enabled(obj, state) obj.funcs->set_drag_enabled(obj.priv_data, state)
#define PUListWidget_set_drop_indicator_shown(obj, state) obj.funcs->set_drop_indicator_shown(obj.priv_data, state)
#define PUListWidget_set_accept_drops(obj, state) obj.funcs->set_accept_drops(obj.priv_data, state)
#define PUListWidget_add_widget_item(obj, item) obj.funcs->add_widget_item(obj.priv_data, item)
#define PUListWidget_set_current_row_changed_event(obj, user_data, event) obj.funcs->set_current_row_changed_event(obj.priv_data, user_data, event)
#define PUListWidget_set_item_clicked_event(obj, user_data, event) obj.funcs->set_item_clicked_event(obj.priv_data, user_data, event)
#define PUListWidget_set_item_double_clicked_event(obj, user_data, event) obj.funcs->set_item_double_clicked_event(obj.priv_data, user_data, event)

#define PULabel_show(obj) obj.funcs->show(obj.priv_data)
#define PULabel_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PULabel_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PULabel_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PULabel_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PULabel_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PULabel_update(obj) obj.funcs->update(obj.priv_data)
#define PULabel_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)

#define PULineEdit_show(obj) obj.funcs->show(obj.priv_data)
#define PULineEdit_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PULineEdit_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PULineEdit_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PULineEdit_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PULineEdit_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PULineEdit_update(obj) obj.funcs->update(obj.priv_data)
#define PULineEdit_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define PULineEdit_set_read_only(obj, value) obj.funcs->set_read_only(obj.priv_data, value)

#define PUPlainTextEdit_show(obj) obj.funcs->show(obj.priv_data)
#define PUPlainTextEdit_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUPlainTextEdit_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUPlainTextEdit_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUPlainTextEdit_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUPlainTextEdit_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUPlainTextEdit_update(obj) obj.funcs->update(obj.priv_data)
#define PUPlainTextEdit_clear(obj) obj.funcs->clear(obj.priv_data)
#define PUPlainTextEdit_set_plain_text(obj, text) obj.funcs->set_plain_text(obj.priv_data, text)
#define PUPlainTextEdit_append_plain_text(obj, text) obj.funcs->append_plain_text(obj.priv_data, text)
#define PUPlainTextEdit_set_read_only(obj, value) obj.funcs->set_read_only(obj.priv_data, value)

#define PUSlider_show(obj) obj.funcs->show(obj.priv_data)
#define PUSlider_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUSlider_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUSlider_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUSlider_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUSlider_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUSlider_update(obj) obj.funcs->update(obj.priv_data)
#define PUSlider_set_value_changed_event(obj, user_data, event) obj.funcs->set_value_changed_event(obj.priv_data, user_data, event)

#define PUMainWindow_show(obj) obj.funcs->show(obj.priv_data)
#define PUMainWindow_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUMainWindow_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUMainWindow_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUMainWindow_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUMainWindow_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUMainWindow_update(obj) obj.funcs->update(obj.priv_data)
#define PUMainWindow_is_animated(obj) obj.funcs->is_animated(obj.priv_data)
#define PUMainWindow_menu_bar(obj) obj.funcs->menu_bar(obj.priv_data)
#define PUMainWindow_set_central_widget(obj, widget) obj.funcs->set_central_widget(obj.priv_data, widget)

#define PUFramelessWindow_show(obj) obj.funcs->show(obj.priv_data)
#define PUFramelessWindow_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUFramelessWindow_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUFramelessWindow_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUFramelessWindow_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUFramelessWindow_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUFramelessWindow_update(obj) obj.funcs->update(obj.priv_data)
#define PUFramelessWindow_set_window_title(obj, title) obj.funcs->set_window_title(obj.priv_data, title)
#define PUFramelessWindow_set_content(obj, widget) obj.funcs->set_content(obj.priv_data, widget)

#define PUAction_is_enabled(obj) obj.funcs->is_enabled(obj.priv_data)
#define PUAction_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define PUAction_set_shortcut(obj, key) obj.funcs->set_shortcut(obj.priv_data, key)
#define PUAction_set_shortcut_mod(obj, key, modifier) obj.funcs->set_shortcut_mod(obj.priv_data, key, modifier)
#define PUAction_set_triggered_event(obj, user_data, event) obj.funcs->set_triggered_event(obj.priv_data, user_data, event)
#define PUAction_set_int_data(obj, data) obj.funcs->set_int_data(obj.priv_data, data)
#define PUAction_get_int_data(obj) obj.funcs->get_int_data(obj.priv_data)

#define PUUrl_is_local_file(obj) obj.funcs->is_local_file(obj.priv_data)
#define PUUrl_to_local_file(obj) obj.funcs->to_local_file(obj.priv_data)

#define PUMimeData_has_color(obj) obj.funcs->has_color(obj.priv_data)
#define PUMimeData_has_image(obj) obj.funcs->has_image(obj.priv_data)
#define PUMimeData_has_text(obj) obj.funcs->has_text(obj.priv_data)
#define PUMimeData_has_urls(obj) obj.funcs->has_urls(obj.priv_data)
#define PUMimeData_urls(obj) obj.funcs->urls(obj.priv_data)

#define PUTimer_set_timeout_event(obj, user_data, event) obj.funcs->set_timeout_event(obj.priv_data, user_data, event)
#define PUTimer_start(obj, time) obj.funcs->start(obj.priv_data, time)

#define PUIcon_add_file(obj, filename) obj.funcs->add_file(obj.priv_data, filename)

#define PUFont_set_family(obj, family) obj.funcs->set_family(obj.priv_data, family)
#define PUFont_set_point_size(obj, size) obj.funcs->set_point_size(obj.priv_data, size)

#define PUMenu_show(obj) obj.funcs->show(obj.priv_data)
#define PUMenu_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUMenu_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUMenu_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUMenu_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUMenu_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUMenu_update(obj) obj.funcs->update(obj.priv_data)
#define PUMenu_add_action_text(obj, text) obj.funcs->add_action_text(obj.priv_data, text)
#define PUMenu_set_triggered_event(obj, user_data, event) obj.funcs->set_triggered_event(obj.priv_data, user_data, event)
#define PUMenu_add_action(obj, action) obj.funcs->add_action(obj.priv_data, action)
#define PUMenu_set_title(obj, title) obj.funcs->set_title(obj.priv_data, title)

#define PUMenuBar_show(obj) obj.funcs->show(obj.priv_data)
#define PUMenuBar_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define PUMenuBar_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define PUMenuBar_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define PUMenuBar_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define PUMenuBar_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define PUMenuBar_update(obj) obj.funcs->update(obj.priv_data)
#define PUMenuBar_add_menu(obj, menu) obj.funcs->add_menu(obj.priv_data, menu)

#define PUApplication_set_style(obj, style) obj.funcs->set_style(obj.priv_data, style)
#define PUApplication_set_style_sheet(obj, filename) obj.funcs->set_style_sheet(obj.priv_data, filename)
#define PUApplication_exec(obj) obj.funcs->exec(obj.priv_data)
#define PUApplication_set_about_to_quit_event(obj, user_data, event) obj.funcs->set_about_to_quit_event(obj.priv_data, user_data, event)
#define PUApplication_get_files(obj) obj.funcs->get_files(obj.priv_data)

#define PUPaintEvent_rect(obj) obj.funcs->rect(obj.priv_data)

#define PUDragEnterEvent_accept(obj) obj.funcs->accept(obj.priv_data)

#define PUDropEvent_accept_proposed_action(obj) obj.funcs->accept_proposed_action(obj.priv_data)
#define PUDropEvent_mime_data(obj) obj.funcs->mime_data(obj.priv_data)

#define PULayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)

#define PUVBoxLayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)
#define PUVBoxLayout_add_layout(obj, layout) obj.funcs->add_layout(obj.priv_data, layout)
#define PUVBoxLayout_update(obj) obj.funcs->update(obj.priv_data)

#define PUHBoxLayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)
#define PUHBoxLayout_add_layout(obj, layout) obj.funcs->add_layout(obj.priv_data, layout)
#define PUHBoxLayout_update(obj) obj.funcs->update(obj.priv_data)

#define PU_create_widget(ui) ui->create_widget(ui->priv_data)
#define PU_create_push_button(ui) ui->create_push_button(ui->priv_data)
#define PU_create_painter(ui) ui->create_painter(ui->priv_data)
#define PU_create_list_widget_item(ui) ui->create_list_widget_item(ui->priv_data)
#define PU_create_list_widget(ui) ui->create_list_widget(ui->priv_data)
#define PU_create_label(ui) ui->create_label(ui->priv_data)
#define PU_create_line_edit(ui) ui->create_line_edit(ui->priv_data)
#define PU_create_plain_text_edit(ui) ui->create_plain_text_edit(ui->priv_data)
#define PU_create_slider(ui) ui->create_slider(ui->priv_data)
#define PU_create_main_window(ui) ui->create_main_window(ui->priv_data)
#define PU_create_frameless_window(ui) ui->create_frameless_window(ui->priv_data)
#define PU_create_action(ui) ui->create_action(ui->priv_data)
#define PU_create_timer(ui) ui->create_timer(ui->priv_data)
#define PU_create_icon(ui) ui->create_icon(ui->priv_data)
#define PU_create_font(ui) ui->create_font(ui->priv_data)
#define PU_create_menu(ui) ui->create_menu(ui->priv_data)
#define PU_create_menu_bar(ui) ui->create_menu_bar(ui->priv_data)
#define PU_create_application(ui) ui->create_application(ui->priv_data)
#define PU_create_v_box_layout(ui) ui->create_v_box_layout(ui->priv_data)
#define PU_create_h_box_layout(ui) ui->create_h_box_layout(ui->priv_data)

#define PU_open_files_dialog(ui) ui->open_files_dialog(obj.priv_data)

#ifdef __cplusplus
}
#endif
