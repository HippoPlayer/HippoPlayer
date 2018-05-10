
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RUBase;

struct RUArray {
    void* priv_data;
    void* elements;
    uint32_t count;
};

struct RURect;
struct RUColor;
struct RUWidget;
struct RUWidgetFuncs;
struct RUPushButton;
struct RUPushButtonFuncs;
struct RUPainter;
struct RUPainterFuncs;
struct RUListWidgetItem;
struct RUListWidgetItemFuncs;
struct RUListWidget;
struct RUListWidgetFuncs;
struct RULabel;
struct RULabelFuncs;
struct RULineEdit;
struct RULineEditFuncs;
struct RUPlainTextEdit;
struct RUPlainTextEditFuncs;
struct RUSlider;
struct RUSliderFuncs;
struct RUMainWindow;
struct RUMainWindowFuncs;
struct RUToolWindowManager;
struct RUToolWindowManagerFuncs;
struct RUDockWidget;
struct RUDockWidgetFuncs;
struct RUDockManager;
struct RUDockManagerFuncs;
struct RUFramelessWindow;
struct RUFramelessWindowFuncs;
struct RUAction;
struct RUActionFuncs;
struct RUUrl;
struct RUUrlFuncs;
struct RUMimeData;
struct RUMimeDataFuncs;
struct RUTimer;
struct RUTimerFuncs;
struct RUIcon;
struct RUIconFuncs;
struct RUFont;
struct RUFontFuncs;
struct RUMenu;
struct RUMenuFuncs;
struct RUMenuBar;
struct RUMenuBarFuncs;
struct RUApplication;
struct RUApplicationFuncs;
struct RUPaintEvent;
struct RUPaintEventFuncs;
struct RUDragEnterEvent;
struct RUDragEnterEventFuncs;
struct RUDropEvent;
struct RUDropEventFuncs;
struct RULayout;
struct RULayoutFuncs;
struct RUVBoxLayout;
struct RUVBoxLayoutFuncs;
struct RUHBoxLayout;
struct RUHBoxLayoutFuncs;
struct RULayoutType;
struct RUPaintDevice;
struct RUWidgetType;

typedef enum RUMetaKeys {
    RUMetaKeys_CTRL,
} RUMetaKeys;

typedef enum RUKeys {
    RUKeys_Key_Escape,
    RUKeys_Key_Tab,
    RUKeys_Key_Backtab,
    RUKeys_Key_Backspace,
    RUKeys_Key_Return,
    RUKeys_Key_Enter,
    RUKeys_Key_Insert,
    RUKeys_Key_Delete,
    RUKeys_Key_Pause,
    RUKeys_Key_Print,
    RUKeys_Key_SysReq,
    RUKeys_Key_Clear,
    RUKeys_Key_Home,
    RUKeys_Key_End,
    RUKeys_Key_Left,
    RUKeys_Key_Up,
    RUKeys_Key_Right,
    RUKeys_Key_Down,
    RUKeys_Key_PageUp,
    RUKeys_Key_PageDown,
    RUKeys_Key_Shift,
    RUKeys_Key_Control,
    RUKeys_Key_Meta,
    RUKeys_Key_Alt,
    RUKeys_Key_CapsLock,
    RUKeys_Key_NumLock,
    RUKeys_Key_ScrollLock,
    RUKeys_Key_F1,
    RUKeys_Key_F2,
    RUKeys_Key_F3,
    RUKeys_Key_F4,
    RUKeys_Key_F5,
    RUKeys_Key_F6,
    RUKeys_Key_F7,
    RUKeys_Key_F8,
    RUKeys_Key_F9,
    RUKeys_Key_F10,
    RUKeys_Key_F11,
    RUKeys_Key_F12,
    RUKeys_Key_F13,
    RUKeys_Key_F14,
    RUKeys_Key_F15,
    RUKeys_Key_F16,
    RUKeys_Key_F17,
    RUKeys_Key_F18,
    RUKeys_Key_F19,
    RUKeys_Key_F20,
    RUKeys_Key_F21,
    RUKeys_Key_F22,
    RUKeys_Key_F23,
    RUKeys_Key_F24,
    RUKeys_Key_F25,
    RUKeys_Key_F26,
    RUKeys_Key_F27,
    RUKeys_Key_F28,
    RUKeys_Key_F29,
    RUKeys_Key_F30,
    RUKeys_Key_F31,
    RUKeys_Key_F32,
    RUKeys_Key_F33,
    RUKeys_Key_F34,
    RUKeys_Key_F35,
    RUKeys_Key_Super_L,
    RUKeys_Key_Super_R,
    RUKeys_Key_Menu,
    RUKeys_Key_Hyper_L,
    RUKeys_Key_Hyper_R,
    RUKeys_Key_Help,
    RUKeys_Key_Direction_L,
    RUKeys_Key_Direction_R,
    RUKeys_Key_Space,
    RUKeys_Key_Any,
    RUKeys_Key_Exclam,
    RUKeys_Key_QuoteDbl,
    RUKeys_Key_NumberSign,
    RUKeys_Key_Dollar,
    RUKeys_Key_Percent,
    RUKeys_Key_Ampersand,
    RUKeys_Key_Apostrophe,
    RUKeys_Key_ParenLeft,
    RUKeys_Key_ParenRight,
    RUKeys_Key_Asterisk,
    RUKeys_Key_Plus,
    RUKeys_Key_Comma,
    RUKeys_Key_Minus,
    RUKeys_Key_Period,
    RUKeys_Key_Slash,
    RUKeys_Key_0,
    RUKeys_Key_1,
    RUKeys_Key_2,
    RUKeys_Key_3,
    RUKeys_Key_4,
    RUKeys_Key_5,
    RUKeys_Key_6,
    RUKeys_Key_7,
    RUKeys_Key_8,
    RUKeys_Key_9,
    RUKeys_Key_Colon,
    RUKeys_Key_Semicolon,
    RUKeys_Key_Less,
    RUKeys_Key_Equal,
    RUKeys_Key_Greater,
    RUKeys_Key_Question,
    RUKeys_Key_At,
    RUKeys_Key_A,
    RUKeys_Key_B,
    RUKeys_Key_C,
    RUKeys_Key_D,
    RUKeys_Key_E,
    RUKeys_Key_F,
    RUKeys_Key_G,
    RUKeys_Key_H,
    RUKeys_Key_I,
    RUKeys_Key_J,
    RUKeys_Key_K,
    RUKeys_Key_L,
    RUKeys_Key_M,
    RUKeys_Key_N,
    RUKeys_Key_O,
    RUKeys_Key_P,
    RUKeys_Key_Q,
    RUKeys_Key_R,
    RUKeys_Key_S,
    RUKeys_Key_T,
    RUKeys_Key_U,
    RUKeys_Key_V,
    RUKeys_Key_W,
    RUKeys_Key_X,
    RUKeys_Key_Y,
    RUKeys_Key_Z,
    RUKeys_Key_BracketLeft,
    RUKeys_Key_Backslash,
    RUKeys_Key_BracketRight,
    RUKeys_Key_AsciiCircum,
    RUKeys_Key_Underscore,
    RUKeys_Key_QuoteLeft,
    RUKeys_Key_BraceLeft,
    RUKeys_Key_Bar,
    RUKeys_Key_BraceRight,
    RUKeys_Key_AsciiTilde,
    RUKeys_Key_Back,
    RUKeys_Key_Forward,
    RUKeys_Key_Stop,
    RUKeys_Key_Refresh,
    RUKeys_Key_VolumeDown,
    RUKeys_Key_VolumeMute,
    RUKeys_Key_VolumeUp,
    RUKeys_Key_BassBoost,
    RUKeys_Key_BassUp,
    RUKeys_Key_BassDown,
    RUKeys_Key_TrebleUp,
    RUKeys_Key_TrebleDown,
    RUKeys_Key_MediaPlay,
    RUKeys_Key_MediaStop,
    RUKeys_Key_MediaPrevious,
    RUKeys_Key_MediaNext,
    RUKeys_Key_MediaRecord,
    RUKeys_Key_MediaPause,
    RUKeys_Key_MediaTogglePlayPause,
    RUKeys_Key_HomePage,
    RUKeys_Key_Favorites,
    RUKeys_Key_Search,
    RUKeys_Key_Standby,
    RUKeys_Key_OpenUrl,
    RUKeys_Key_LaunchMail,
    RUKeys_Key_LaunchMedia,
    RUKeys_Key_Launch0,
    RUKeys_Key_Launch1,
    RUKeys_Key_Launch2,
    RUKeys_Key_Launch3,
    RUKeys_Key_Launch4,
    RUKeys_Key_Launch5,
    RUKeys_Key_Launch6,
    RUKeys_Key_Launch7,
    RUKeys_Key_Launch8,
    RUKeys_Key_Launch9,
    RUKeys_Key_LaunchA,
    RUKeys_Key_LaunchB,
    RUKeys_Key_LaunchC,
    RUKeys_Key_LaunchD,
    RUKeys_Key_LaunchE,
    RUKeys_Key_LaunchF,
    RUKeys_Key_MonBrightnessUp,
    RUKeys_Key_MonBrightnessDown,
    RUKeys_Key_KeyboardLightOnOff,
    RUKeys_Key_KeyboardBrightnessUp,
    RUKeys_Key_KeyboardBrightnessDown,
    RUKeys_Key_PowerOff,
    RUKeys_Key_WakeUp,
    RUKeys_Key_Eject,
    RUKeys_Key_ScreenSaver,
    RUKeys_Key_WWW,
    RUKeys_Key_Memo,
    RUKeys_Key_LightBulb,
    RUKeys_Key_Shop,
    RUKeys_Key_History,
    RUKeys_Key_AddFavorite,
    RUKeys_Key_HotLinks,
    RUKeys_Key_BrightnessAdjust,
    RUKeys_Key_Finance,
    RUKeys_Key_Community,
    RUKeys_Key_AudioRewind,
    RUKeys_Key_BackForward,
    RUKeys_Key_ApplicationLeft,
    RUKeys_Key_ApplicationRight,
    RUKeys_Key_Book,
    RUKeys_Key_CD,
    RUKeys_Key_Calculator,
    RUKeys_Key_ToDoList,
    RUKeys_Key_ClearGrab,
    RUKeys_Key_Close,
    RUKeys_Key_Copy,
    RUKeys_Key_Cut,
    RUKeys_Key_Display,
    RUKeys_Key_DOS,
    RUKeys_Key_Documents,
    RUKeys_Key_Excel,
    RUKeys_Key_Explorer,
    RUKeys_Key_Game,
    RUKeys_Key_Go,
    RUKeys_Key_iTouch,
    RUKeys_Key_LogOff,
    RUKeys_Key_Market,
    RUKeys_Key_Meeting,
    RUKeys_Key_MenuKB,
    RUKeys_Key_MenuPB,
    RUKeys_Key_MySites,
    RUKeys_Key_News,
    RUKeys_Key_OfficeHome,
    RUKeys_Key_Option,
    RUKeys_Key_Paste,
    RUKeys_Key_Phone,
    RUKeys_Key_Calendar,
    RUKeys_Key_Reply,
    RUKeys_Key_Reload,
    RUKeys_Key_RotateWindows,
    RUKeys_Key_RotationPB,
    RUKeys_Key_RotationKB,
    RUKeys_Key_Save,
    RUKeys_Key_Send,
    RUKeys_Key_Spell,
    RUKeys_Key_SplitScreen,
    RUKeys_Key_Support,
    RUKeys_Key_TaskPane,
    RUKeys_Key_Terminal,
    RUKeys_Key_Tools,
    RUKeys_Key_Travel,
    RUKeys_Key_Video,
    RUKeys_Key_Word,
    RUKeys_Key_Xfer,
    RUKeys_Key_ZoomIn,
    RUKeys_Key_ZoomOut,
    RUKeys_Key_Away,
    RUKeys_Key_Messenger,
    RUKeys_Key_WebCam,
    RUKeys_Key_MailForward,
    RUKeys_Key_Pictures,
    RUKeys_Key_Music,
    RUKeys_Key_Battery,
    RUKeys_Key_Bluetooth,
    RUKeys_Key_WLAN,
    RUKeys_Key_UWB,
    RUKeys_Key_AudioForward,
    RUKeys_Key_AudioRepeat,
    RUKeys_Key_AudioRandomPlay,
    RUKeys_Key_Subtitle,
    RUKeys_Key_AudioCycleTrack,
    RUKeys_Key_Time,
    RUKeys_Key_Hibernate,
    RUKeys_Key_View,
    RUKeys_Key_TopMenu,
    RUKeys_Key_PowerDown,
    RUKeys_Key_Suspend,
    RUKeys_Key_ContrastAdjust,
    RUKeys_Key_LaunchG,
    RUKeys_Key_LaunchH,
    RUKeys_Key_TouchpadToggle,
    RUKeys_Key_TouchpadOn,
    RUKeys_Key_TouchpadOff,
    RUKeys_Key_MicMute,
    RUKeys_Key_Red,
    RUKeys_Key_Green,
    RUKeys_Key_Yellow,
    RUKeys_Key_Blue,
    RUKeys_Key_ChannelUp,
    RUKeys_Key_ChannelDown,
    RUKeys_Key_Guide,
    RUKeys_Key_Info,
    RUKeys_Key_Settings,
    RUKeys_Key_MicVolumeUp,
    RUKeys_Key_MicVolumeDown,
    RUKeys_Key_New,
    RUKeys_Key_Open,
    RUKeys_Key_Find,
    RUKeys_Key_Undo,
    RUKeys_Key_Redo,
    RUKeys_Key_MediaLast,
    RUKeys_Key_Select,
    RUKeys_Key_Yes,
    RUKeys_Key_No,
    RUKeys_Key_Cancel,
    RUKeys_Key_Printer,
    RUKeys_Key_Execute,
    RUKeys_Key_Sleep,
    RUKeys_Key_Play,
    RUKeys_Key_Zoom,
    RUKeys_Key_Exit,
    RUKeys_Key_Context1,
    RUKeys_Key_Context2,
    RUKeys_Key_Context3,
    RUKeys_Key_Context4,
    RUKeys_Key_Call,
    RUKeys_Key_Hangup,
    RUKeys_Key_Flip,
    RUKeys_Key_ToggleCallHangup,
    RUKeys_Key_VoiceDial,
    RUKeys_Key_LastNumberRedial,
    RUKeys_Key_Camera,
    RUKeys_Key_CameraFocus,
} RUKeys;

struct RURect {
    int x;
    int y;
    int width;
    int height;
};

struct RUColor {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t a;
};

struct RUWidgetFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_paint_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* event));
};

struct RUWidget {
    struct RUWidgetFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUPushButtonFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_pressed_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_released_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_icon)(struct RUBase* self_c, struct RUBase* icon);
    void (*set_text)(struct RUBase* self_c, const char* text);
    void (*set_flat)(struct RUBase* self_c, bool flat);
};

struct RUPushButton {
    struct RUPushButtonFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUPainterFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*begin)(struct RUBase* self_c, struct RUBase* target);
    void (*end)(struct RUBase* self_c);
    void (*set_font)(struct RUBase* self_c, struct RUBase* font);
    void (*draw_text)(struct RUBase* self_c, int x, int y, const char* text);
    void (*draw_line)(struct RUBase* self_c, int x1, int y1, int x2, int y2);
    void (*fill_rect_color)(struct RUBase* self_c, struct RURect rect, struct RUColor color);
};

struct RUPainter {
    struct RUPainterFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUListWidgetItemFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*set_text)(struct RUBase* self_c, const char* text);
    const char* (*text)(struct RUBase* self_c);
    void (*set_string_data)(struct RUBase* self_c, const char* text);
    const char* (*get_string_data)(struct RUBase* self_c);
};

struct RUListWidgetItem {
    struct RUListWidgetItemFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUListWidgetFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*add_item)(struct RUBase* self_c, struct RUBase* item);
    void (*add_text_item)(struct RUBase* self_c, const char* text);
    struct RUListWidgetItem (*current_item)(struct RUBase* self_c);
    int (*current_row)(struct RUBase* self_c);
    struct RUArray (*selected_items)(struct RUBase* self_c);
    struct RUListWidgetItem (*item)(struct RUBase* self_c, int index);
    void (*set_current_row)(struct RUBase* self_c, int index);
    int (*count)(struct RUBase* self_c);
    void (*set_drag_enabled)(struct RUBase* self_c, bool state);
    void (*set_drop_indicator_shown)(struct RUBase* self_c, bool state);
    void (*set_accept_drops)(struct RUBase* self_c, bool state);
    void (*add_widget_item)(struct RUBase* self_c, struct RUBase* item);
    void (*set_current_row_changed_event)(void* object, void* user_data, void (*event)(void* self_c, int row));
    void (*set_item_clicked_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* item));
    void (*set_item_double_clicked_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* item));
    void (*set_drag_enter_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* event));
    void (*set_drop_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* event));
};

struct RUListWidget {
    struct RUListWidgetFuncs* funcs;
    struct RUBase* priv_data;
};

struct RULabelFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_text)(struct RUBase* self_c, const char* text);
};

struct RULabel {
    struct RULabelFuncs* funcs;
    struct RUBase* priv_data;
};

struct RULineEditFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_text)(struct RUBase* self_c, const char* text);
    void (*set_read_only)(struct RUBase* self_c, bool value);
};

struct RULineEdit {
    struct RULineEditFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUPlainTextEditFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*clear)(struct RUBase* self_c);
    void (*set_plain_text)(struct RUBase* self_c, const char* text);
    void (*append_plain_text)(struct RUBase* self_c, const char* text);
    void (*set_read_only)(struct RUBase* self_c, bool value);
};

struct RUPlainTextEdit {
    struct RUPlainTextEditFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUSliderFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_value_changed_event)(void* object, void* user_data, void (*event)(void* self_c, int value));
};

struct RUSlider {
    struct RUSliderFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUMainWindowFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    bool (*is_animated)(struct RUBase* self_c);
    struct RUMenuBar (*menu_bar)(struct RUBase* self_c);
    void (*set_central_widget)(struct RUBase* self_c, struct RUBase* widget);
};

struct RUMainWindow {
    struct RUMainWindowFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUToolWindowManagerFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    const char* (*save_state)(struct RUBase* self_c);
    void (*restore_state)(struct RUBase* self_c, const char* state);
    void (*add_to_docking)(struct RUBase* self_c, struct RUBase* widget);
    void (*add_to_docking_floating)(struct RUBase* self_c, struct RUBase* widget);
};

struct RUToolWindowManager {
    struct RUToolWindowManagerFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUDockWidgetFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*set_widget)(struct RUBase* self_c, struct RUBase* widget);
};

struct RUDockWidget {
    struct RUDockWidgetFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUDockManagerFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*save)(struct RUBase* self_c);
    void (*add_to_docking)(struct RUBase* self_c, struct RUBase* widget);
};

struct RUDockManager {
    struct RUDockManagerFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUFramelessWindowFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*set_window_title)(struct RUBase* self_c, const char* title);
    void (*set_content)(struct RUBase* self_c, struct RUBase* widget);
};

struct RUFramelessWindow {
    struct RUFramelessWindowFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUActionFuncs {
    void (*destroy)(struct RUBase* self_c);
    bool (*is_enabled)(struct RUBase* self_c);
    void (*set_text)(struct RUBase* self_c, const char* text);
    void (*set_shortcut)(struct RUBase* self_c, RUKeys key);
    void (*set_shortcut_mod)(struct RUBase* self_c, RUKeys key, RUMetaKeys modifier);
    void (*set_triggered_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*set_int_data)(struct RUBase* self_c, int data);
    int (*get_int_data)(struct RUBase* self_c);
};

struct RUAction {
    struct RUActionFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUUrlFuncs {
    bool (*is_local_file)(struct RUBase* self_c);
    const char* (*to_local_file)(struct RUBase* self_c);
};

struct RUUrl {
    struct RUUrlFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUMimeDataFuncs {
    bool (*has_color)(struct RUBase* self_c);
    bool (*has_image)(struct RUBase* self_c);
    bool (*has_text)(struct RUBase* self_c);
    bool (*has_urls)(struct RUBase* self_c);
    struct RUArray (*urls)(struct RUBase* self_c);
};

struct RUMimeData {
    struct RUMimeDataFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUTimerFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*set_timeout_event)(void* object, void* user_data, void (*event)(void* self_c));
    void (*start)(struct RUBase* self_c, int time);
};

struct RUTimer {
    struct RUTimerFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUIconFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*add_file)(struct RUBase* self_c, const char* filename);
};

struct RUIcon {
    struct RUIconFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUFontFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*set_family)(struct RUBase* self_c, const char* family);
    void (*set_point_size)(struct RUBase* self_c, int size);
};

struct RUFont {
    struct RUFontFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUMenuFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*add_action_text)(struct RUBase* self_c, const char* text);
    void (*set_triggered_event)(void* object, void* user_data, void (*event)(void* self_c, struct RUBase* action));
    void (*add_action)(struct RUBase* self_c, struct RUBase* action);
    void (*set_title)(struct RUBase* self_c, const char* title);
};

struct RUMenu {
    struct RUMenuFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUMenuBarFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*show)(struct RUBase* self_c);
    void (*set_persist_data)(struct RUBase* self_c, const char* text);
    const char* (*persist_data)(struct RUBase* self_c);
    void (*set_fixed_height)(struct RUBase* self_c, int width);
    void (*set_fixed_width)(struct RUBase* self_c, int width);
    void (*resize)(struct RUBase* self_c, int width, int height);
    void (*set_parent)(struct RUBase* self_c, struct RUBase* widget);
    void (*set_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
    void (*add_menu)(struct RUBase* self_c, struct RUBase* menu);
};

struct RUMenuBar {
    struct RUMenuBarFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUApplicationFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*set_style)(struct RUBase* self_c, const char* style);
    int (*set_style_sheet)(struct RUBase* self_c, const char* filename);
    void (*exec)(struct RUBase* self_c);
    void (*set_about_to_quit_event)(void* object, void* user_data, void (*event)(void* self_c));
    struct RUArray (*get_files)(struct RUBase* self_c);
};

struct RUApplication {
    struct RUApplicationFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUPaintEventFuncs {
    struct RURect (*rect)(struct RUBase* self_c);
};

struct RUPaintEvent {
    struct RUPaintEventFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUDragEnterEventFuncs {
    void (*accept)(struct RUBase* self_c);
};

struct RUDragEnterEvent {
    struct RUDragEnterEventFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUDropEventFuncs {
    void (*accept_proposed_action)(struct RUBase* self_c);
    struct RUMimeData (*mime_data)(struct RUBase* self_c);
};

struct RUDropEvent {
    struct RUDropEventFuncs* funcs;
    struct RUBase* priv_data;
};

struct RULayoutFuncs {
    void (*add_widget)(struct RUBase* self_c, struct RUBase* widget);
};

struct RULayout {
    struct RULayoutFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUVBoxLayoutFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*add_widget)(struct RUBase* self_c, struct RUBase* widget);
    void (*add_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
};

struct RUVBoxLayout {
    struct RUVBoxLayoutFuncs* funcs;
    struct RUBase* priv_data;
};

struct RUHBoxLayoutFuncs {
    void (*destroy)(struct RUBase* self_c);
    void (*add_widget)(struct RUBase* self_c, struct RUBase* widget);
    void (*add_layout)(struct RUBase* self_c, struct RUBase* layout);
    void (*update)(struct RUBase* self_c);
};

struct RUHBoxLayout {
    struct RUHBoxLayoutFuncs* funcs;
    struct RUBase* priv_data;
};

typedef struct RUPluginUI { 
    struct RUWidget (*create_widget)(struct RUBase* self);
    struct RUPushButton (*create_push_button)(struct RUBase* self);
    struct RUPainter (*create_painter)(struct RUBase* self);
    struct RUListWidgetItem (*create_list_widget_item)(struct RUBase* self);
    struct RUListWidget (*create_list_widget)(struct RUBase* self);
    struct RULabel (*create_label)(struct RUBase* self);
    struct RULineEdit (*create_line_edit)(struct RUBase* self);
    struct RUPlainTextEdit (*create_plain_text_edit)(struct RUBase* self);
    struct RUSlider (*create_slider)(struct RUBase* self);
    struct RUMainWindow (*create_main_window)(struct RUBase* self);
    struct RUDockWidget (*create_dock_widget)(struct RUBase* self);
    struct RUFramelessWindow (*create_frameless_window)(struct RUBase* self);
    struct RUAction (*create_action)(struct RUBase* self);
    struct RUTimer (*create_timer)(struct RUBase* self);
    struct RUIcon (*create_icon)(struct RUBase* self);
    struct RUFont (*create_font)(struct RUBase* self);
    struct RUMenu (*create_menu)(struct RUBase* self);
    struct RUMenuBar (*create_menu_bar)(struct RUBase* self);
    struct RUVBoxLayout (*create_v_box_layout)(struct RUBase* self);
    struct RUHBoxLayout (*create_h_box_layout)(struct RUBase* self);
    struct RUArray (*open_files_dialog)(struct RUBase* self_c);
    struct RUWidget (*get_parent)(struct RUBase* self);
    struct RUBase* priv_data;
} RUPlugin;

typedef struct RU { 
    struct RUWidget (*create_widget)(struct RUBase* self);
    struct RUPushButton (*create_push_button)(struct RUBase* self);
    struct RUPainter (*create_painter)(struct RUBase* self);
    struct RUListWidgetItem (*create_list_widget_item)(struct RUBase* self);
    struct RUListWidget (*create_list_widget)(struct RUBase* self);
    struct RULabel (*create_label)(struct RUBase* self);
    struct RULineEdit (*create_line_edit)(struct RUBase* self);
    struct RUPlainTextEdit (*create_plain_text_edit)(struct RUBase* self);
    struct RUSlider (*create_slider)(struct RUBase* self);
    struct RUMainWindow (*create_main_window)(struct RUBase* self);
    struct RUToolWindowManager (*create_tool_window_manager)(struct RUBase* self);
    struct RUDockWidget (*create_dock_widget)(struct RUBase* self);
    struct RUDockManager (*create_dock_manager)(struct RUBase* self);
    struct RUFramelessWindow (*create_frameless_window)(struct RUBase* self);
    struct RUAction (*create_action)(struct RUBase* self);
    struct RUTimer (*create_timer)(struct RUBase* self);
    struct RUIcon (*create_icon)(struct RUBase* self);
    struct RUFont (*create_font)(struct RUBase* self);
    struct RUMenu (*create_menu)(struct RUBase* self);
    struct RUMenuBar (*create_menu_bar)(struct RUBase* self);
    struct RUApplication (*create_application)(struct RUBase* self);
    struct RUVBoxLayout (*create_v_box_layout)(struct RUBase* self);
    struct RUHBoxLayout (*create_h_box_layout)(struct RUBase* self);
    struct RUArray (*open_files_dialog)(struct RUBase* self_c);
    struct RUPluginUI* (*create_plugin_ui)(struct RUBase* self, struct RUBase* parent);
    void (*destroy_plugin_ui)(struct RUPluginUI* self);
    struct RUBase* priv_data;
} RU;

#define RUWidget_show(obj) obj.funcs->show(obj.priv_data)
#define RUWidget_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUWidget_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUWidget_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUWidget_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUWidget_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUWidget_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUWidget_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUWidget_update(obj) obj.funcs->update(obj.priv_data)

#define RUPushButton_show(obj) obj.funcs->show(obj.priv_data)
#define RUPushButton_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUPushButton_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUPushButton_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUPushButton_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUPushButton_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUPushButton_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUPushButton_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUPushButton_update(obj) obj.funcs->update(obj.priv_data)
#define RUPushButton_set_pressed_event(obj, user_data, event) obj.funcs->set_pressed_event(obj.priv_data, user_data, event)
#define RUPushButton_set_released_event(obj, user_data, event) obj.funcs->set_released_event(obj.priv_data, user_data, event)
#define RUPushButton_set_icon(obj, icon) obj.funcs->set_icon(obj.priv_data, icon)
#define RUPushButton_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define RUPushButton_set_flat(obj, flat) obj.funcs->set_flat(obj.priv_data, flat)

#define RUPainter_begin(obj, target) obj.funcs->begin(obj.priv_data, target)
#define RUPainter_end(obj) obj.funcs->end(obj.priv_data)
#define RUPainter_set_font(obj, font) obj.funcs->set_font(obj.priv_data, font)
#define RUPainter_draw_text(obj, x, y, text) obj.funcs->draw_text(obj.priv_data, x, y, text)
#define RUPainter_draw_line(obj, x1, y1, x2, y2) obj.funcs->draw_line(obj.priv_data, x1, y1, x2, y2)
#define RUPainter_fill_rect_color(obj, rect, color) obj.funcs->fill_rect_color(obj.priv_data, rect, color)

#define RUListWidgetItem_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define RUListWidgetItem_text(obj) obj.funcs->text(obj.priv_data)
#define RUListWidgetItem_set_string_data(obj, text) obj.funcs->set_string_data(obj.priv_data, text)
#define RUListWidgetItem_get_string_data(obj) obj.funcs->get_string_data(obj.priv_data)

#define RUListWidget_show(obj) obj.funcs->show(obj.priv_data)
#define RUListWidget_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUListWidget_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUListWidget_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUListWidget_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUListWidget_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUListWidget_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUListWidget_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUListWidget_update(obj) obj.funcs->update(obj.priv_data)
#define RUListWidget_add_item(obj, item) obj.funcs->add_item(obj.priv_data, item)
#define RUListWidget_add_text_item(obj, text) obj.funcs->add_text_item(obj.priv_data, text)
#define RUListWidget_current_item(obj) obj.funcs->current_item(obj.priv_data)
#define RUListWidget_current_row(obj) obj.funcs->current_row(obj.priv_data)
#define RUListWidget_selected_items(obj) obj.funcs->selected_items(obj.priv_data)
#define RUListWidget_item(obj, index) obj.funcs->item(obj.priv_data, index)
#define RUListWidget_set_current_row(obj, index) obj.funcs->set_current_row(obj.priv_data, index)
#define RUListWidget_count(obj) obj.funcs->count(obj.priv_data)
#define RUListWidget_set_drag_enabled(obj, state) obj.funcs->set_drag_enabled(obj.priv_data, state)
#define RUListWidget_set_drop_indicator_shown(obj, state) obj.funcs->set_drop_indicator_shown(obj.priv_data, state)
#define RUListWidget_set_accept_drops(obj, state) obj.funcs->set_accept_drops(obj.priv_data, state)
#define RUListWidget_add_widget_item(obj, item) obj.funcs->add_widget_item(obj.priv_data, item)
#define RUListWidget_set_current_row_changed_event(obj, user_data, event) obj.funcs->set_current_row_changed_event(obj.priv_data, user_data, event)
#define RUListWidget_set_item_clicked_event(obj, user_data, event) obj.funcs->set_item_clicked_event(obj.priv_data, user_data, event)
#define RUListWidget_set_item_double_clicked_event(obj, user_data, event) obj.funcs->set_item_double_clicked_event(obj.priv_data, user_data, event)

#define RULabel_show(obj) obj.funcs->show(obj.priv_data)
#define RULabel_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RULabel_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RULabel_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RULabel_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RULabel_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RULabel_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RULabel_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RULabel_update(obj) obj.funcs->update(obj.priv_data)
#define RULabel_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)

#define RULineEdit_show(obj) obj.funcs->show(obj.priv_data)
#define RULineEdit_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RULineEdit_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RULineEdit_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RULineEdit_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RULineEdit_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RULineEdit_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RULineEdit_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RULineEdit_update(obj) obj.funcs->update(obj.priv_data)
#define RULineEdit_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define RULineEdit_set_read_only(obj, value) obj.funcs->set_read_only(obj.priv_data, value)

#define RUPlainTextEdit_show(obj) obj.funcs->show(obj.priv_data)
#define RUPlainTextEdit_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUPlainTextEdit_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUPlainTextEdit_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUPlainTextEdit_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUPlainTextEdit_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUPlainTextEdit_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUPlainTextEdit_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUPlainTextEdit_update(obj) obj.funcs->update(obj.priv_data)
#define RUPlainTextEdit_clear(obj) obj.funcs->clear(obj.priv_data)
#define RUPlainTextEdit_set_plain_text(obj, text) obj.funcs->set_plain_text(obj.priv_data, text)
#define RUPlainTextEdit_append_plain_text(obj, text) obj.funcs->append_plain_text(obj.priv_data, text)
#define RUPlainTextEdit_set_read_only(obj, value) obj.funcs->set_read_only(obj.priv_data, value)

#define RUSlider_show(obj) obj.funcs->show(obj.priv_data)
#define RUSlider_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUSlider_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUSlider_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUSlider_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUSlider_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUSlider_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUSlider_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUSlider_update(obj) obj.funcs->update(obj.priv_data)
#define RUSlider_set_value_changed_event(obj, user_data, event) obj.funcs->set_value_changed_event(obj.priv_data, user_data, event)

#define RUMainWindow_show(obj) obj.funcs->show(obj.priv_data)
#define RUMainWindow_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUMainWindow_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUMainWindow_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUMainWindow_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUMainWindow_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUMainWindow_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUMainWindow_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUMainWindow_update(obj) obj.funcs->update(obj.priv_data)
#define RUMainWindow_is_animated(obj) obj.funcs->is_animated(obj.priv_data)
#define RUMainWindow_menu_bar(obj) obj.funcs->menu_bar(obj.priv_data)
#define RUMainWindow_set_central_widget(obj, widget) obj.funcs->set_central_widget(obj.priv_data, widget)

#define RUToolWindowManager_show(obj) obj.funcs->show(obj.priv_data)
#define RUToolWindowManager_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUToolWindowManager_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUToolWindowManager_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUToolWindowManager_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUToolWindowManager_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUToolWindowManager_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUToolWindowManager_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUToolWindowManager_update(obj) obj.funcs->update(obj.priv_data)
#define RUToolWindowManager_save_state(obj) obj.funcs->save_state(obj.priv_data)
#define RUToolWindowManager_restore_state(obj, state) obj.funcs->restore_state(obj.priv_data, state)
#define RUToolWindowManager_add_to_docking(obj, widget) obj.funcs->add_to_docking(obj.priv_data, widget)
#define RUToolWindowManager_add_to_docking_floating(obj, widget) obj.funcs->add_to_docking_floating(obj.priv_data, widget)

#define RUDockWidget_set_widget(obj, widget) obj.funcs->set_widget(obj.priv_data, widget)

#define RUDockManager_show(obj) obj.funcs->show(obj.priv_data)
#define RUDockManager_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUDockManager_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUDockManager_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUDockManager_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUDockManager_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUDockManager_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUDockManager_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUDockManager_update(obj) obj.funcs->update(obj.priv_data)
#define RUDockManager_save(obj) obj.funcs->save(obj.priv_data)
#define RUDockManager_add_to_docking(obj, widget) obj.funcs->add_to_docking(obj.priv_data, widget)

#define RUFramelessWindow_show(obj) obj.funcs->show(obj.priv_data)
#define RUFramelessWindow_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUFramelessWindow_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUFramelessWindow_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUFramelessWindow_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUFramelessWindow_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUFramelessWindow_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUFramelessWindow_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUFramelessWindow_update(obj) obj.funcs->update(obj.priv_data)
#define RUFramelessWindow_set_window_title(obj, title) obj.funcs->set_window_title(obj.priv_data, title)
#define RUFramelessWindow_set_content(obj, widget) obj.funcs->set_content(obj.priv_data, widget)

#define RUAction_is_enabled(obj) obj.funcs->is_enabled(obj.priv_data)
#define RUAction_set_text(obj, text) obj.funcs->set_text(obj.priv_data, text)
#define RUAction_set_shortcut(obj, key) obj.funcs->set_shortcut(obj.priv_data, key)
#define RUAction_set_shortcut_mod(obj, key, modifier) obj.funcs->set_shortcut_mod(obj.priv_data, key, modifier)
#define RUAction_set_triggered_event(obj, user_data, event) obj.funcs->set_triggered_event(obj.priv_data, user_data, event)
#define RUAction_set_int_data(obj, data) obj.funcs->set_int_data(obj.priv_data, data)
#define RUAction_get_int_data(obj) obj.funcs->get_int_data(obj.priv_data)

#define RUUrl_is_local_file(obj) obj.funcs->is_local_file(obj.priv_data)
#define RUUrl_to_local_file(obj) obj.funcs->to_local_file(obj.priv_data)

#define RUMimeData_has_color(obj) obj.funcs->has_color(obj.priv_data)
#define RUMimeData_has_image(obj) obj.funcs->has_image(obj.priv_data)
#define RUMimeData_has_text(obj) obj.funcs->has_text(obj.priv_data)
#define RUMimeData_has_urls(obj) obj.funcs->has_urls(obj.priv_data)
#define RUMimeData_urls(obj) obj.funcs->urls(obj.priv_data)

#define RUTimer_set_timeout_event(obj, user_data, event) obj.funcs->set_timeout_event(obj.priv_data, user_data, event)
#define RUTimer_start(obj, time) obj.funcs->start(obj.priv_data, time)

#define RUIcon_add_file(obj, filename) obj.funcs->add_file(obj.priv_data, filename)

#define RUFont_set_family(obj, family) obj.funcs->set_family(obj.priv_data, family)
#define RUFont_set_point_size(obj, size) obj.funcs->set_point_size(obj.priv_data, size)

#define RUMenu_show(obj) obj.funcs->show(obj.priv_data)
#define RUMenu_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUMenu_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUMenu_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUMenu_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUMenu_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUMenu_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUMenu_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUMenu_update(obj) obj.funcs->update(obj.priv_data)
#define RUMenu_add_action_text(obj, text) obj.funcs->add_action_text(obj.priv_data, text)
#define RUMenu_set_triggered_event(obj, user_data, event) obj.funcs->set_triggered_event(obj.priv_data, user_data, event)
#define RUMenu_add_action(obj, action) obj.funcs->add_action(obj.priv_data, action)
#define RUMenu_set_title(obj, title) obj.funcs->set_title(obj.priv_data, title)

#define RUMenuBar_show(obj) obj.funcs->show(obj.priv_data)
#define RUMenuBar_set_persist_data(obj, text) obj.funcs->set_persist_data(obj.priv_data, text)
#define RUMenuBar_persist_data(obj) obj.funcs->persist_data(obj.priv_data)
#define RUMenuBar_set_fixed_height(obj, width) obj.funcs->set_fixed_height(obj.priv_data, width)
#define RUMenuBar_set_fixed_width(obj, width) obj.funcs->set_fixed_width(obj.priv_data, width)
#define RUMenuBar_resize(obj, width, height) obj.funcs->resize(obj.priv_data, width, height)
#define RUMenuBar_set_parent(obj, widget) obj.funcs->set_parent(obj.priv_data, widget)
#define RUMenuBar_set_layout(obj, layout) obj.funcs->set_layout(obj.priv_data, layout)
#define RUMenuBar_update(obj) obj.funcs->update(obj.priv_data)
#define RUMenuBar_add_menu(obj, menu) obj.funcs->add_menu(obj.priv_data, menu)

#define RUApplication_set_style(obj, style) obj.funcs->set_style(obj.priv_data, style)
#define RUApplication_set_style_sheet(obj, filename) obj.funcs->set_style_sheet(obj.priv_data, filename)
#define RUApplication_exec(obj) obj.funcs->exec(obj.priv_data)
#define RUApplication_set_about_to_quit_event(obj, user_data, event) obj.funcs->set_about_to_quit_event(obj.priv_data, user_data, event)
#define RUApplication_get_files(obj) obj.funcs->get_files(obj.priv_data)

#define RUPaintEvent_rect(obj) obj.funcs->rect(obj.priv_data)

#define RUDragEnterEvent_accept(obj) obj.funcs->accept(obj.priv_data)

#define RUDropEvent_accept_proposed_action(obj) obj.funcs->accept_proposed_action(obj.priv_data)
#define RUDropEvent_mime_data(obj) obj.funcs->mime_data(obj.priv_data)

#define RULayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)

#define RUVBoxLayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)
#define RUVBoxLayout_add_layout(obj, layout) obj.funcs->add_layout(obj.priv_data, layout)
#define RUVBoxLayout_update(obj) obj.funcs->update(obj.priv_data)

#define RUHBoxLayout_add_widget(obj, widget) obj.funcs->add_widget(obj.priv_data, widget)
#define RUHBoxLayout_add_layout(obj, layout) obj.funcs->add_layout(obj.priv_data, layout)
#define RUHBoxLayout_update(obj) obj.funcs->update(obj.priv_data)

#define RU_create_widget(ui) ui->create_widget(ui->priv_data)
#define RU_create_push_button(ui) ui->create_push_button(ui->priv_data)
#define RU_create_painter(ui) ui->create_painter(ui->priv_data)
#define RU_create_list_widget_item(ui) ui->create_list_widget_item(ui->priv_data)
#define RU_create_list_widget(ui) ui->create_list_widget(ui->priv_data)
#define RU_create_label(ui) ui->create_label(ui->priv_data)
#define RU_create_line_edit(ui) ui->create_line_edit(ui->priv_data)
#define RU_create_plain_text_edit(ui) ui->create_plain_text_edit(ui->priv_data)
#define RU_create_slider(ui) ui->create_slider(ui->priv_data)
#define RU_create_main_window(ui) ui->create_main_window(ui->priv_data)
#define RU_create_tool_window_manager(ui) ui->create_tool_window_manager(ui->priv_data)
#define RU_create_dock_widget(ui) ui->create_dock_widget(ui->priv_data)
#define RU_create_dock_manager(ui) ui->create_dock_manager(ui->priv_data)
#define RU_create_frameless_window(ui) ui->create_frameless_window(ui->priv_data)
#define RU_create_action(ui) ui->create_action(ui->priv_data)
#define RU_create_timer(ui) ui->create_timer(ui->priv_data)
#define RU_create_icon(ui) ui->create_icon(ui->priv_data)
#define RU_create_font(ui) ui->create_font(ui->priv_data)
#define RU_create_menu(ui) ui->create_menu(ui->priv_data)
#define RU_create_menu_bar(ui) ui->create_menu_bar(ui->priv_data)
#define RU_create_application(ui) ui->create_application(ui->priv_data)
#define RU_create_v_box_layout(ui) ui->create_v_box_layout(ui->priv_data)
#define RU_create_h_box_layout(ui) ui->create_h_box_layout(ui->priv_data)

#define RU_open_files_dialog(ui) ui->open_files_dialog(obj.priv_data)

#ifdef __cplusplus
}
#endif
