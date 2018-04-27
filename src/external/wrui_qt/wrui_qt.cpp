#include "c_api.h"
#include "qt_api_gen.h"
#include <assert.h>
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QListWidgetItem>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSlider>
#include <QMainWindow>
#include <QToolWindowManager>
#include <QFramelessWindow>
#include <QAction>
#include <QUrl>
#include <QMimeData>
#include <QTimer>
#include <QIcon>
#include <QFont>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QPaintEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

static char s_temp_string_buffer[8192];

struct PrivData {
    QWidget* parent;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct PUWidgetFuncs s_widget_funcs;
extern struct PUPushButtonFuncs s_push_button_funcs;
extern struct PUPainterFuncs s_painter_funcs;
extern struct PUListWidgetItemFuncs s_list_widget_item_funcs;
extern struct PUListWidgetFuncs s_list_widget_funcs;
extern struct PULabelFuncs s_label_funcs;
extern struct PULineEditFuncs s_line_edit_funcs;
extern struct PUPlainTextEditFuncs s_plain_text_edit_funcs;
extern struct PUSliderFuncs s_slider_funcs;
extern struct PUMainWindowFuncs s_main_window_funcs;
extern struct PUToolWindowManagerFuncs s_tool_window_manager_funcs;
extern struct PUFramelessWindowFuncs s_frameless_window_funcs;
extern struct PUActionFuncs s_action_funcs;
extern struct PUUrlFuncs s_url_funcs;
extern struct PUMimeDataFuncs s_mime_data_funcs;
extern struct PUTimerFuncs s_timer_funcs;
extern struct PUIconFuncs s_icon_funcs;
extern struct PUFontFuncs s_font_funcs;
extern struct PUMenuFuncs s_menu_funcs;
extern struct PUMenuBarFuncs s_menu_bar_funcs;
extern struct PUApplicationFuncs s_application_funcs;
extern struct PUPaintEventFuncs s_paint_event_funcs;
extern struct PUDragEnterEventFuncs s_drag_enter_event_funcs;
extern struct PUDropEventFuncs s_drop_event_funcs;
extern struct PULayoutFuncs s_layout_funcs;
extern struct PUVBoxLayoutFuncs s_v_box_layout_funcs;
extern struct PUHBoxLayoutFuncs s_h_box_layout_funcs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct KeyVal { int val, key; };
static std::map<int, int> s_meta_keys_lookup;
static std::map<int, int> s_keys_lookup;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void create_enum_mappings() {
    static KeyVal meta_keys_vals[] = {
        { (int)Qt::CTRL, 0 },
    };

    for (int i = 0; i < 1; ++i) {
        s_meta_keys_lookup[meta_keys_vals[i].key] = meta_keys_vals[i].val;
    };
    static KeyVal keys_vals[] = {
        { (int)Qt::Key_Escape, 0 },
        { (int)Qt::Key_Tab, 1 },
        { (int)Qt::Key_Backtab, 2 },
        { (int)Qt::Key_Backspace, 3 },
        { (int)Qt::Key_Return, 4 },
        { (int)Qt::Key_Enter, 5 },
        { (int)Qt::Key_Insert, 6 },
        { (int)Qt::Key_Delete, 7 },
        { (int)Qt::Key_Pause, 8 },
        { (int)Qt::Key_Print, 9 },
        { (int)Qt::Key_SysReq, 10 },
        { (int)Qt::Key_Clear, 11 },
        { (int)Qt::Key_Home, 12 },
        { (int)Qt::Key_End, 13 },
        { (int)Qt::Key_Left, 14 },
        { (int)Qt::Key_Up, 15 },
        { (int)Qt::Key_Right, 16 },
        { (int)Qt::Key_Down, 17 },
        { (int)Qt::Key_PageUp, 18 },
        { (int)Qt::Key_PageDown, 19 },
        { (int)Qt::Key_Shift, 20 },
        { (int)Qt::Key_Control, 21 },
        { (int)Qt::Key_Meta, 22 },
        { (int)Qt::Key_Alt, 23 },
        { (int)Qt::Key_CapsLock, 24 },
        { (int)Qt::Key_NumLock, 25 },
        { (int)Qt::Key_ScrollLock, 26 },
        { (int)Qt::Key_F1, 27 },
        { (int)Qt::Key_F2, 28 },
        { (int)Qt::Key_F3, 29 },
        { (int)Qt::Key_F4, 30 },
        { (int)Qt::Key_F5, 31 },
        { (int)Qt::Key_F6, 32 },
        { (int)Qt::Key_F7, 33 },
        { (int)Qt::Key_F8, 34 },
        { (int)Qt::Key_F9, 35 },
        { (int)Qt::Key_F10, 36 },
        { (int)Qt::Key_F11, 37 },
        { (int)Qt::Key_F12, 38 },
        { (int)Qt::Key_F13, 39 },
        { (int)Qt::Key_F14, 40 },
        { (int)Qt::Key_F15, 41 },
        { (int)Qt::Key_F16, 42 },
        { (int)Qt::Key_F17, 43 },
        { (int)Qt::Key_F18, 44 },
        { (int)Qt::Key_F19, 45 },
        { (int)Qt::Key_F20, 46 },
        { (int)Qt::Key_F21, 47 },
        { (int)Qt::Key_F22, 48 },
        { (int)Qt::Key_F23, 49 },
        { (int)Qt::Key_F24, 50 },
        { (int)Qt::Key_F25, 51 },
        { (int)Qt::Key_F26, 52 },
        { (int)Qt::Key_F27, 53 },
        { (int)Qt::Key_F28, 54 },
        { (int)Qt::Key_F29, 55 },
        { (int)Qt::Key_F30, 56 },
        { (int)Qt::Key_F31, 57 },
        { (int)Qt::Key_F32, 58 },
        { (int)Qt::Key_F33, 59 },
        { (int)Qt::Key_F34, 60 },
        { (int)Qt::Key_F35, 61 },
        { (int)Qt::Key_Super_L, 62 },
        { (int)Qt::Key_Super_R, 63 },
        { (int)Qt::Key_Menu, 64 },
        { (int)Qt::Key_Hyper_L, 65 },
        { (int)Qt::Key_Hyper_R, 66 },
        { (int)Qt::Key_Help, 67 },
        { (int)Qt::Key_Direction_L, 68 },
        { (int)Qt::Key_Direction_R, 69 },
        { (int)Qt::Key_Space, 70 },
        { (int)Qt::Key_Any, 71 },
        { (int)Qt::Key_Exclam, 72 },
        { (int)Qt::Key_QuoteDbl, 73 },
        { (int)Qt::Key_NumberSign, 74 },
        { (int)Qt::Key_Dollar, 75 },
        { (int)Qt::Key_Percent, 76 },
        { (int)Qt::Key_Ampersand, 77 },
        { (int)Qt::Key_Apostrophe, 78 },
        { (int)Qt::Key_ParenLeft, 79 },
        { (int)Qt::Key_ParenRight, 80 },
        { (int)Qt::Key_Asterisk, 81 },
        { (int)Qt::Key_Plus, 82 },
        { (int)Qt::Key_Comma, 83 },
        { (int)Qt::Key_Minus, 84 },
        { (int)Qt::Key_Period, 85 },
        { (int)Qt::Key_Slash, 86 },
        { (int)Qt::Key_0, 87 },
        { (int)Qt::Key_1, 88 },
        { (int)Qt::Key_2, 89 },
        { (int)Qt::Key_3, 90 },
        { (int)Qt::Key_4, 91 },
        { (int)Qt::Key_5, 92 },
        { (int)Qt::Key_6, 93 },
        { (int)Qt::Key_7, 94 },
        { (int)Qt::Key_8, 95 },
        { (int)Qt::Key_9, 96 },
        { (int)Qt::Key_Colon, 97 },
        { (int)Qt::Key_Semicolon, 98 },
        { (int)Qt::Key_Less, 99 },
        { (int)Qt::Key_Equal, 100 },
        { (int)Qt::Key_Greater, 101 },
        { (int)Qt::Key_Question, 102 },
        { (int)Qt::Key_At, 103 },
        { (int)Qt::Key_A, 104 },
        { (int)Qt::Key_B, 105 },
        { (int)Qt::Key_C, 106 },
        { (int)Qt::Key_D, 107 },
        { (int)Qt::Key_E, 108 },
        { (int)Qt::Key_F, 109 },
        { (int)Qt::Key_G, 110 },
        { (int)Qt::Key_H, 111 },
        { (int)Qt::Key_I, 112 },
        { (int)Qt::Key_J, 113 },
        { (int)Qt::Key_K, 114 },
        { (int)Qt::Key_L, 115 },
        { (int)Qt::Key_M, 116 },
        { (int)Qt::Key_N, 117 },
        { (int)Qt::Key_O, 118 },
        { (int)Qt::Key_P, 119 },
        { (int)Qt::Key_Q, 120 },
        { (int)Qt::Key_R, 121 },
        { (int)Qt::Key_S, 122 },
        { (int)Qt::Key_T, 123 },
        { (int)Qt::Key_U, 124 },
        { (int)Qt::Key_V, 125 },
        { (int)Qt::Key_W, 126 },
        { (int)Qt::Key_X, 127 },
        { (int)Qt::Key_Y, 128 },
        { (int)Qt::Key_Z, 129 },
        { (int)Qt::Key_BracketLeft, 130 },
        { (int)Qt::Key_Backslash, 131 },
        { (int)Qt::Key_BracketRight, 132 },
        { (int)Qt::Key_AsciiCircum, 133 },
        { (int)Qt::Key_Underscore, 134 },
        { (int)Qt::Key_QuoteLeft, 135 },
        { (int)Qt::Key_BraceLeft, 136 },
        { (int)Qt::Key_Bar, 137 },
        { (int)Qt::Key_BraceRight, 138 },
        { (int)Qt::Key_AsciiTilde, 139 },
        { (int)Qt::Key_Back, 140 },
        { (int)Qt::Key_Forward, 141 },
        { (int)Qt::Key_Stop, 142 },
        { (int)Qt::Key_Refresh, 143 },
        { (int)Qt::Key_VolumeDown, 144 },
        { (int)Qt::Key_VolumeMute, 145 },
        { (int)Qt::Key_VolumeUp, 146 },
        { (int)Qt::Key_BassBoost, 147 },
        { (int)Qt::Key_BassUp, 148 },
        { (int)Qt::Key_BassDown, 149 },
        { (int)Qt::Key_TrebleUp, 150 },
        { (int)Qt::Key_TrebleDown, 151 },
        { (int)Qt::Key_MediaPlay, 152 },
        { (int)Qt::Key_MediaStop, 153 },
        { (int)Qt::Key_MediaPrevious, 154 },
        { (int)Qt::Key_MediaNext, 155 },
        { (int)Qt::Key_MediaRecord, 156 },
        { (int)Qt::Key_MediaPause, 157 },
        { (int)Qt::Key_MediaTogglePlayPause, 158 },
        { (int)Qt::Key_HomePage, 159 },
        { (int)Qt::Key_Favorites, 160 },
        { (int)Qt::Key_Search, 161 },
        { (int)Qt::Key_Standby, 162 },
        { (int)Qt::Key_OpenUrl, 163 },
        { (int)Qt::Key_LaunchMail, 164 },
        { (int)Qt::Key_LaunchMedia, 165 },
        { (int)Qt::Key_Launch0, 166 },
        { (int)Qt::Key_Launch1, 167 },
        { (int)Qt::Key_Launch2, 168 },
        { (int)Qt::Key_Launch3, 169 },
        { (int)Qt::Key_Launch4, 170 },
        { (int)Qt::Key_Launch5, 171 },
        { (int)Qt::Key_Launch6, 172 },
        { (int)Qt::Key_Launch7, 173 },
        { (int)Qt::Key_Launch8, 174 },
        { (int)Qt::Key_Launch9, 175 },
        { (int)Qt::Key_LaunchA, 176 },
        { (int)Qt::Key_LaunchB, 177 },
        { (int)Qt::Key_LaunchC, 178 },
        { (int)Qt::Key_LaunchD, 179 },
        { (int)Qt::Key_LaunchE, 180 },
        { (int)Qt::Key_LaunchF, 181 },
        { (int)Qt::Key_MonBrightnessUp, 182 },
        { (int)Qt::Key_MonBrightnessDown, 183 },
        { (int)Qt::Key_KeyboardLightOnOff, 184 },
        { (int)Qt::Key_KeyboardBrightnessUp, 185 },
        { (int)Qt::Key_KeyboardBrightnessDown, 186 },
        { (int)Qt::Key_PowerOff, 187 },
        { (int)Qt::Key_WakeUp, 188 },
        { (int)Qt::Key_Eject, 189 },
        { (int)Qt::Key_ScreenSaver, 190 },
        { (int)Qt::Key_WWW, 191 },
        { (int)Qt::Key_Memo, 192 },
        { (int)Qt::Key_LightBulb, 193 },
        { (int)Qt::Key_Shop, 194 },
        { (int)Qt::Key_History, 195 },
        { (int)Qt::Key_AddFavorite, 196 },
        { (int)Qt::Key_HotLinks, 197 },
        { (int)Qt::Key_BrightnessAdjust, 198 },
        { (int)Qt::Key_Finance, 199 },
        { (int)Qt::Key_Community, 200 },
        { (int)Qt::Key_AudioRewind, 201 },
        { (int)Qt::Key_BackForward, 202 },
        { (int)Qt::Key_ApplicationLeft, 203 },
        { (int)Qt::Key_ApplicationRight, 204 },
        { (int)Qt::Key_Book, 205 },
        { (int)Qt::Key_CD, 206 },
        { (int)Qt::Key_Calculator, 207 },
        { (int)Qt::Key_ToDoList, 208 },
        { (int)Qt::Key_ClearGrab, 209 },
        { (int)Qt::Key_Close, 210 },
        { (int)Qt::Key_Copy, 211 },
        { (int)Qt::Key_Cut, 212 },
        { (int)Qt::Key_Display, 213 },
        { (int)Qt::Key_DOS, 214 },
        { (int)Qt::Key_Documents, 215 },
        { (int)Qt::Key_Excel, 216 },
        { (int)Qt::Key_Explorer, 217 },
        { (int)Qt::Key_Game, 218 },
        { (int)Qt::Key_Go, 219 },
        { (int)Qt::Key_iTouch, 220 },
        { (int)Qt::Key_LogOff, 221 },
        { (int)Qt::Key_Market, 222 },
        { (int)Qt::Key_Meeting, 223 },
        { (int)Qt::Key_MenuKB, 224 },
        { (int)Qt::Key_MenuPB, 225 },
        { (int)Qt::Key_MySites, 226 },
        { (int)Qt::Key_News, 227 },
        { (int)Qt::Key_OfficeHome, 228 },
        { (int)Qt::Key_Option, 229 },
        { (int)Qt::Key_Paste, 230 },
        { (int)Qt::Key_Phone, 231 },
        { (int)Qt::Key_Calendar, 232 },
        { (int)Qt::Key_Reply, 233 },
        { (int)Qt::Key_Reload, 234 },
        { (int)Qt::Key_RotateWindows, 235 },
        { (int)Qt::Key_RotationPB, 236 },
        { (int)Qt::Key_RotationKB, 237 },
        { (int)Qt::Key_Save, 238 },
        { (int)Qt::Key_Send, 239 },
        { (int)Qt::Key_Spell, 240 },
        { (int)Qt::Key_SplitScreen, 241 },
        { (int)Qt::Key_Support, 242 },
        { (int)Qt::Key_TaskPane, 243 },
        { (int)Qt::Key_Terminal, 244 },
        { (int)Qt::Key_Tools, 245 },
        { (int)Qt::Key_Travel, 246 },
        { (int)Qt::Key_Video, 247 },
        { (int)Qt::Key_Word, 248 },
        { (int)Qt::Key_Xfer, 249 },
        { (int)Qt::Key_ZoomIn, 250 },
        { (int)Qt::Key_ZoomOut, 251 },
        { (int)Qt::Key_Away, 252 },
        { (int)Qt::Key_Messenger, 253 },
        { (int)Qt::Key_WebCam, 254 },
        { (int)Qt::Key_MailForward, 255 },
        { (int)Qt::Key_Pictures, 256 },
        { (int)Qt::Key_Music, 257 },
        { (int)Qt::Key_Battery, 258 },
        { (int)Qt::Key_Bluetooth, 259 },
        { (int)Qt::Key_WLAN, 260 },
        { (int)Qt::Key_UWB, 261 },
        { (int)Qt::Key_AudioForward, 262 },
        { (int)Qt::Key_AudioRepeat, 263 },
        { (int)Qt::Key_AudioRandomPlay, 264 },
        { (int)Qt::Key_Subtitle, 265 },
        { (int)Qt::Key_AudioCycleTrack, 266 },
        { (int)Qt::Key_Time, 267 },
        { (int)Qt::Key_Hibernate, 268 },
        { (int)Qt::Key_View, 269 },
        { (int)Qt::Key_TopMenu, 270 },
        { (int)Qt::Key_PowerDown, 271 },
        { (int)Qt::Key_Suspend, 272 },
        { (int)Qt::Key_ContrastAdjust, 273 },
        { (int)Qt::Key_LaunchG, 274 },
        { (int)Qt::Key_LaunchH, 275 },
        { (int)Qt::Key_TouchpadToggle, 276 },
        { (int)Qt::Key_TouchpadOn, 277 },
        { (int)Qt::Key_TouchpadOff, 278 },
        { (int)Qt::Key_MicMute, 279 },
        { (int)Qt::Key_Red, 280 },
        { (int)Qt::Key_Green, 281 },
        { (int)Qt::Key_Yellow, 282 },
        { (int)Qt::Key_Blue, 283 },
        { (int)Qt::Key_ChannelUp, 284 },
        { (int)Qt::Key_ChannelDown, 285 },
        { (int)Qt::Key_Guide, 286 },
        { (int)Qt::Key_Info, 287 },
        { (int)Qt::Key_Settings, 288 },
        { (int)Qt::Key_MicVolumeUp, 289 },
        { (int)Qt::Key_MicVolumeDown, 290 },
        { (int)Qt::Key_New, 291 },
        { (int)Qt::Key_Open, 292 },
        { (int)Qt::Key_Find, 293 },
        { (int)Qt::Key_Undo, 294 },
        { (int)Qt::Key_Redo, 295 },
        { (int)Qt::Key_MediaLast, 296 },
        { (int)Qt::Key_Select, 297 },
        { (int)Qt::Key_Yes, 298 },
        { (int)Qt::Key_No, 299 },
        { (int)Qt::Key_Cancel, 300 },
        { (int)Qt::Key_Printer, 301 },
        { (int)Qt::Key_Execute, 302 },
        { (int)Qt::Key_Sleep, 303 },
        { (int)Qt::Key_Play, 304 },
        { (int)Qt::Key_Zoom, 305 },
        { (int)Qt::Key_Exit, 306 },
        { (int)Qt::Key_Context1, 307 },
        { (int)Qt::Key_Context2, 308 },
        { (int)Qt::Key_Context3, 309 },
        { (int)Qt::Key_Context4, 310 },
        { (int)Qt::Key_Call, 311 },
        { (int)Qt::Key_Hangup, 312 },
        { (int)Qt::Key_Flip, 313 },
        { (int)Qt::Key_ToggleCallHangup, 314 },
        { (int)Qt::Key_VoiceDial, 315 },
        { (int)Qt::Key_LastNumberRedial, 316 },
        { (int)Qt::Key_Camera, 317 },
        { (int)Qt::Key_CameraFocus, 318 },
    };

    for (int i = 0; i < 319; ++i) {
        s_keys_lookup[keys_vals[i].key] = keys_vals[i].val;
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRWidget : public QWidget {
public:
    WRWidget(QWidget* widget) : QWidget(widget) {}
    virtual ~WRWidget() {}

protected:
    virtual void paintEvent(QPaintEvent* event) {
        if (m_paint) {
            PUPaintEvent e;
            e.funcs = &s_paint_event_funcs;
            e.priv_data = (struct PUBase*)event;
            m_paint(m_paint_user_data, (struct PUBase*)&e);
        } else {
            QWidget::paintEvent(event);
        }
    }

public:
    void (*m_paint)(void* self_c, struct PUBase* event) = nullptr;
    void* m_paint_user_data = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_widget_paint_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event)) {
    WRWidget* qt_object = (WRWidget*)object;
    qt_object->m_paint_user_data = user_data;
    qt_object->m_paint = event;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRPushButton : public QPushButton {
public:
    WRPushButton(QWidget* widget) : QPushButton(widget) {}
    virtual ~WRPushButton() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRListWidget : public QListWidget {
public:
    WRListWidget(QWidget* widget) : QListWidget(widget) {}
    virtual ~WRListWidget() {}

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event) {
        if (m_drag_enter) {
            PUDragEnterEvent e;
            e.funcs = &s_drag_enter_event_funcs;
            e.priv_data = (struct PUBase*)event;
            m_drag_enter(m_drag_enter_user_data, (struct PUBase*)&e);
        } else {
            QListWidget::dragEnterEvent(event);
        }
    }

public:
    void (*m_drag_enter)(void* self_c, struct PUBase* event) = nullptr;
    void* m_drag_enter_user_data = nullptr;
protected:
    virtual void dropEvent(QDropEvent* event) {
        if (m_drop) {
            PUDropEvent e;
            e.funcs = &s_drop_event_funcs;
            e.priv_data = (struct PUBase*)event;
            m_drop(m_drop_user_data, (struct PUBase*)&e);
        } else {
            QListWidget::dropEvent(event);
        }
    }

public:
    void (*m_drop)(void* self_c, struct PUBase* event) = nullptr;
    void* m_drop_user_data = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_list_widget_drag_enter_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event)) {
    WRListWidget* qt_object = (WRListWidget*)object;
    qt_object->m_drag_enter_user_data = user_data;
    qt_object->m_drag_enter = event;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_list_widget_drop_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* event)) {
    WRListWidget* qt_object = (WRListWidget*)object;
    qt_object->m_drop_user_data = user_data;
    qt_object->m_drop = event;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRLabel : public QLabel {
public:
    WRLabel(QWidget* widget) : QLabel(widget) {}
    virtual ~WRLabel() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRLineEdit : public QLineEdit {
public:
    WRLineEdit(QWidget* widget) : QLineEdit(widget) {}
    virtual ~WRLineEdit() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRPlainTextEdit : public QPlainTextEdit {
public:
    WRPlainTextEdit(QWidget* widget) : QPlainTextEdit(widget) {}
    virtual ~WRPlainTextEdit() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRSlider : public QSlider {
public:
    WRSlider(QWidget* widget) : QSlider(widget) {}
    virtual ~WRSlider() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMainWindow : public QMainWindow {
public:
    WRMainWindow(QWidget* widget) : QMainWindow(widget) {}
    virtual ~WRMainWindow() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRToolWindowManager : public QToolWindowManager {
public:
    WRToolWindowManager(QWidget* widget) : QToolWindowManager(widget) {}
    virtual ~WRToolWindowManager() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRFramelessWindow : public QFramelessWindow {
public:
    WRFramelessWindow(QWidget* widget) : QFramelessWindow(widget) {}
    virtual ~WRFramelessWindow() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMenu : public QMenu {
public:
    WRMenu(QWidget* widget) : QMenu(widget) {}
    virtual ~WRMenu() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMenuBar : public QMenuBar {
public:
    WRMenuBar(QWidget* widget) : QMenuBar(widget) {}
    virtual ~WRMenuBar() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRVBoxLayout : public QVBoxLayout {
public:
    WRVBoxLayout(QWidget* widget) : QVBoxLayout(widget) {}
    virtual ~WRVBoxLayout() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRHBoxLayout : public QHBoxLayout {
public:
    WRHBoxLayout(QWidget* widget) : QHBoxLayout(widget) {}
    virtual ~WRHBoxLayout() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_show(struct PUBase* self_c) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_fixed_height(struct PUBase* self_c, int width) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_fixed_width(struct PUBase* self_c, int width) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_resize(struct PUBase* self_c, int width, int height) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_update(struct PUBase* self_c) { 
    WRWidget* qt_data = (WRWidget*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_show(struct PUBase* self_c) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_fixed_height(struct PUBase* self_c, int width) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_fixed_width(struct PUBase* self_c, int width) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_resize(struct PUBase* self_c, int width, int height) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_update(struct PUBase* self_c) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_push_button_pressed_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(pressed()), wrap, SLOT(method()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_push_button_released_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(released()), wrap, SLOT(method()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_icon(struct PUBase* self_c, struct PUBase* icon) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setIcon(*(QIcon*)icon);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_text(struct PUBase* self_c, const char* text) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void push_button_set_flat(struct PUBase* self_c, bool flat) { 
    WRPushButton* qt_data = (WRPushButton*)self_c;
    qt_data->setFlat(flat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_begin(struct PUBase* self_c, struct PUBase* target) { 
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->begin(dynamic_cast<QPaintDevice*>((QObject*)target));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_end(struct PUBase* self_c) { 
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->end();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_set_font(struct PUBase* self_c, struct PUBase* font) { 
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->setFont(*(QFont*)font);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_draw_text(struct PUBase* self_c, int x, int y, const char* text) { 
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->drawText(x, y, QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_draw_line(struct PUBase* self_c, int x1, int y1, int x2, int y2) { 
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->drawLine(x1, y1, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_item_set_text(struct PUBase* self_c, const char* text) { 
    QListWidgetItem* qt_data = (QListWidgetItem*)self_c;
    qt_data->setText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* list_widget_item_text(struct PUBase* self_c) { 
    QListWidgetItem* qt_data = (QListWidgetItem*)self_c;
    auto ret_value = qt_data->text();
    QByteArray ba = ret_value.toUtf8();
    const char* c_str = ba.data();
    assert((ba.size() + 1) < sizeof(s_temp_string_buffer));
    memcpy(s_temp_string_buffer, c_str, ba.size() + 1);
    printf("temp string buffer %s\n", s_temp_string_buffer);
    return s_temp_string_buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_show(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_fixed_height(struct PUBase* self_c, int width) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_fixed_width(struct PUBase* self_c, int width) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_resize(struct PUBase* self_c, int width, int height) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_update(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_add_item(struct PUBase* self_c, struct PUBase* item) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->addItem((QListWidgetItem*)item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUListWidgetItem list_widget_current_item(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    auto ret_value = qt_data->currentItem();
    PUListWidgetItem ctl;
    ctl.funcs = &s_list_widget_item_funcs;
    ctl.priv_data = (struct PUBase*)ret_value;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int list_widget_current_row(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    auto ret_value = qt_data->currentRow();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUArray list_widget_selected_items(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    auto ret_value = qt_data->selectedItems();
    int count = ret_value.size();
    PUArray array = { 0 };
    if (count > 0) {
        PUListWidgetItem* elements = new PUListWidgetItem[count];
        for (int i = 0; i < count; ++i) {
            elements[i].funcs = &s_list_widget_item_funcs;
            elements[i].priv_data = (struct PUBase*)ret_value.at(i);
       }
       array.elements = (void*)elements;
       array.count = int(count);
   }
   return array;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUListWidgetItem list_widget_item(struct PUBase* self_c, int index) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    auto ret_value = qt_data->item(index);
    PUListWidgetItem ctl;
    ctl.funcs = &s_list_widget_item_funcs;
    ctl.priv_data = (struct PUBase*)ret_value;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_current_row(struct PUBase* self_c, int index) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setCurrentRow(index);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int list_widget_count(struct PUBase* self_c) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    auto ret_value = qt_data->count();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_drag_enabled(struct PUBase* self_c, bool state) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setDragEnabled(state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_drop_indicator_shown(struct PUBase* self_c, bool state) { 
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->setDropIndicatorShown(state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_list_widget_current_row_changed_event(void* object, void* user_data, void (*event)(void* self_c, int row)) {
    QSlotWrapperSignal_self_i32_void* wrap = new QSlotWrapperSignal_self_i32_void(user_data, (Signal_self_i32_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(currentRowChanged(int)), wrap, SLOT(method(int)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_list_widget_item_clicked_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* item)) {
    QSlotWrapperSignal_self_ListWidgetItem_void* wrap = new QSlotWrapperSignal_self_ListWidgetItem_void(user_data, (Signal_self_ListWidgetItem_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(itemClicked(QListWidgetItem*)), wrap, SLOT(method(QListWidgetItem*)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_list_widget_item_double_clicked_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* item)) {
    QSlotWrapperSignal_self_ListWidgetItem_void* wrap = new QSlotWrapperSignal_self_ListWidgetItem_void(user_data, (Signal_self_ListWidgetItem_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(itemDoubleClicked(QListWidgetItem*)), wrap, SLOT(method(QListWidgetItem*)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_show(struct PUBase* self_c) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_set_fixed_height(struct PUBase* self_c, int width) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_set_fixed_width(struct PUBase* self_c, int width) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_resize(struct PUBase* self_c, int width, int height) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_update(struct PUBase* self_c) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void label_set_text(struct PUBase* self_c, const char* text) { 
    WRLabel* qt_data = (WRLabel*)self_c;
    qt_data->setText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_show(struct PUBase* self_c) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_fixed_height(struct PUBase* self_c, int width) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_fixed_width(struct PUBase* self_c, int width) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_resize(struct PUBase* self_c, int width, int height) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_update(struct PUBase* self_c) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_text(struct PUBase* self_c, const char* text) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void line_edit_set_read_only(struct PUBase* self_c, bool value) { 
    WRLineEdit* qt_data = (WRLineEdit*)self_c;
    qt_data->setReadOnly(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_show(struct PUBase* self_c) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_fixed_height(struct PUBase* self_c, int width) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_fixed_width(struct PUBase* self_c, int width) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_resize(struct PUBase* self_c, int width, int height) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_update(struct PUBase* self_c) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_clear(struct PUBase* self_c) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_plain_text(struct PUBase* self_c, const char* text) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setPlainText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_append_plain_text(struct PUBase* self_c, const char* text) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->appendPlainText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void plain_text_edit_set_read_only(struct PUBase* self_c, bool value) { 
    WRPlainTextEdit* qt_data = (WRPlainTextEdit*)self_c;
    qt_data->setReadOnly(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_show(struct PUBase* self_c) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_set_fixed_height(struct PUBase* self_c, int width) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_set_fixed_width(struct PUBase* self_c, int width) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_resize(struct PUBase* self_c, int width, int height) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void slider_update(struct PUBase* self_c) { 
    WRSlider* qt_data = (WRSlider*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_slider_value_changed_event(void* object, void* user_data, void (*event)(void* self_c, int value)) {
    QSlotWrapperSignal_self_i32_void* wrap = new QSlotWrapperSignal_self_i32_void(user_data, (Signal_self_i32_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(valueChanged(int)), wrap, SLOT(method(int)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_show(struct PUBase* self_c) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_set_fixed_height(struct PUBase* self_c, int width) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_set_fixed_width(struct PUBase* self_c, int width) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_resize(struct PUBase* self_c, int width, int height) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_update(struct PUBase* self_c) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool main_window_is_animated(struct PUBase* self_c) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    auto ret_value = qt_data->isAnimated();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUMenuBar main_window_menu_bar(struct PUBase* self_c) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    auto ret_value = qt_data->menuBar();
    PUMenuBar ctl;
    ctl.funcs = &s_menu_bar_funcs;
    ctl.priv_data = (struct PUBase*)ret_value;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void main_window_set_central_widget(struct PUBase* self_c, struct PUBase* widget) { 
    WRMainWindow* qt_data = (WRMainWindow*)self_c;
    qt_data->setCentralWidget((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_show(struct PUBase* self_c) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_set_fixed_height(struct PUBase* self_c, int width) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_set_fixed_width(struct PUBase* self_c, int width) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_resize(struct PUBase* self_c, int width, int height) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_update(struct PUBase* self_c) { 
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_show(struct PUBase* self_c) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_fixed_height(struct PUBase* self_c, int width) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_fixed_width(struct PUBase* self_c, int width) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_resize(struct PUBase* self_c, int width, int height) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_update(struct PUBase* self_c) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_window_title(struct PUBase* self_c, const char* title) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setWindowTitle(QString::fromUtf8(title));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void frameless_window_set_content(struct PUBase* self_c, struct PUBase* widget) { 
    WRFramelessWindow* qt_data = (WRFramelessWindow*)self_c;
    qt_data->setContent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool action_is_enabled(struct PUBase* self_c) { 
    QAction* qt_data = (QAction*)self_c;
    auto ret_value = qt_data->isEnabled();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void action_set_text(struct PUBase* self_c, const char* text) { 
    QAction* qt_data = (QAction*)self_c;
    qt_data->setText(QString::fromUtf8(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_action_triggered_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(triggered()), wrap, SLOT(method()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool url_is_local_file(struct PUBase* self_c) { 
    QUrl* qt_data = (QUrl*)self_c;
    auto ret_value = qt_data->isLocalFile();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* url_to_local_file(struct PUBase* self_c) { 
    QUrl* qt_data = (QUrl*)self_c;
    auto ret_value = qt_data->toLocalFile();
    QByteArray ba = ret_value.toUtf8();
    const char* c_str = ba.data();
    assert((ba.size() + 1) < sizeof(s_temp_string_buffer));
    memcpy(s_temp_string_buffer, c_str, ba.size() + 1);
    printf("temp string buffer %s\n", s_temp_string_buffer);
    return s_temp_string_buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool mime_data_has_color(struct PUBase* self_c) { 
    QMimeData* qt_data = (QMimeData*)self_c;
    auto ret_value = qt_data->hasColor();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool mime_data_has_image(struct PUBase* self_c) { 
    QMimeData* qt_data = (QMimeData*)self_c;
    auto ret_value = qt_data->hasImage();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool mime_data_has_text(struct PUBase* self_c) { 
    QMimeData* qt_data = (QMimeData*)self_c;
    auto ret_value = qt_data->hasText();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool mime_data_has_urls(struct PUBase* self_c) { 
    QMimeData* qt_data = (QMimeData*)self_c;
    auto ret_value = qt_data->hasUrls();
    return ret_value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUArray mime_data_urls(struct PUBase* self_c) { 
    QMimeData* qt_data = (QMimeData*)self_c;
    auto ret_value = qt_data->urls();
    int count = ret_value.size();
    PUArray array = { 0 };
    if (count > 0) {
        PUUrl* elements = new PUUrl[count];
        for (int i = 0; i < count; ++i) {
            elements[i].funcs = &s_url_funcs;
            QUrl* temp = new QUrl(ret_value.at(i));
            elements[i].priv_data = (struct PUBase*)temp;
       }
       array.elements = (void*)elements;
       array.count = int(count);
   }
   return array;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_timer_timeout_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(timeout()), wrap, SLOT(method()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void timer_start(struct PUBase* self_c, int time) { 
    QTimer* qt_data = (QTimer*)self_c;
    qt_data->start(time);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void icon_add_file(struct PUBase* self_c, const char* filename) { 
    QIcon* qt_data = (QIcon*)self_c;
    qt_data->addFile(QString::fromUtf8(filename));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void font_set_family(struct PUBase* self_c, const char* family) { 
    QFont* qt_data = (QFont*)self_c;
    qt_data->setFamily(QString::fromUtf8(family));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void font_set_point_size(struct PUBase* self_c, int size) { 
    QFont* qt_data = (QFont*)self_c;
    qt_data->setPointSize(size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_show(struct PUBase* self_c) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_fixed_height(struct PUBase* self_c, int width) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_fixed_width(struct PUBase* self_c, int width) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_resize(struct PUBase* self_c, int width, int height) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_update(struct PUBase* self_c) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_menu_triggered_event(void* object, void* user_data, void (*event)(void* self_c, struct PUBase* action)) {
    QSlotWrapperSignal_self_Action_void* wrap = new QSlotWrapperSignal_self_Action_void(user_data, (Signal_self_Action_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(triggered(QAction*)), wrap, SLOT(method(QAction*)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_add_action(struct PUBase* self_c, struct PUBase* action) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->addAction((QAction*)action);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_title(struct PUBase* self_c, const char* title) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setTitle(QString::fromUtf8(title));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_show(struct PUBase* self_c) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_set_fixed_height(struct PUBase* self_c, int width) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->setFixedHeight(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_set_fixed_width(struct PUBase* self_c, int width) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->setFixedWidth(width);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_resize(struct PUBase* self_c, int width, int height) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_set_parent(struct PUBase* self_c, struct PUBase* widget) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->setParent((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_set_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->setLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_update(struct PUBase* self_c) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_bar_add_menu(struct PUBase* self_c, struct PUBase* menu) { 
    WRMenuBar* qt_data = (WRMenuBar*)self_c;
    qt_data->addMenu((QMenu*)menu);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void application_set_style(struct PUBase* self_c, const char* style) { 
    QApplication* qt_data = (QApplication*)self_c;
    qt_data->setStyle(QString::fromUtf8(style));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void application_exec(struct PUBase* self_c) { 
    QApplication* qt_data = (QApplication*)self_c;
    qt_data->exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_application_about_to_quit_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(aboutToQuit()), wrap, SLOT(method()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PURect paint_event_rect(struct PUBase* self_c) { 
    QPaintEvent* qt_data = (QPaintEvent*)self_c;
    const auto& t = qt_data->rect();
    return PURect { t.x(), t.y(), t.width(), t.height() };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void drag_enter_event_accept(struct PUBase* self_c) { 
    QDragEnterEvent* qt_data = (QDragEnterEvent*)self_c;
    qt_data->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void drop_event_accept_proposed_action(struct PUBase* self_c) { 
    QDropEvent* qt_data = (QDropEvent*)self_c;
    qt_data->acceptProposedAction();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUMimeData drop_event_mime_data(struct PUBase* self_c) { 
    QDropEvent* qt_data = (QDropEvent*)self_c;
    auto ret_value = qt_data->mimeData();
    PUMimeData ctl;
    ctl.funcs = &s_mime_data_funcs;
    ctl.priv_data = (struct PUBase*)ret_value;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void layout_add_widget(struct PUBase* self_c, struct PUBase* widget) { 
    QLayout* qt_data = (QLayout*)self_c;
    qt_data->addWidget((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void v_box_layout_add_widget(struct PUBase* self_c, struct PUBase* widget) { 
    QVBoxLayout* qt_data = (QVBoxLayout*)self_c;
    qt_data->addWidget((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void v_box_layout_add_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRVBoxLayout* qt_data = (WRVBoxLayout*)self_c;
    qt_data->addLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void v_box_layout_update(struct PUBase* self_c) { 
    WRVBoxLayout* qt_data = (WRVBoxLayout*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void h_box_layout_add_widget(struct PUBase* self_c, struct PUBase* widget) { 
    QHBoxLayout* qt_data = (QHBoxLayout*)self_c;
    qt_data->addWidget((QWidget*)widget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void h_box_layout_add_layout(struct PUBase* self_c, struct PUBase* layout) { 
    WRHBoxLayout* qt_data = (WRHBoxLayout*)self_c;
    qt_data->addLayout((QLayout*)layout);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void h_box_layout_update(struct PUBase* self_c) { 
    WRHBoxLayout* qt_data = (WRHBoxLayout*)self_c;
    qt_data->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename F, typename QT> T create_widget_func(F* funcs, void* priv_data) {
    PrivData* data = (PrivData*)priv_data;
    QT* qt_obj = nullptr;
    if (data) {
        qt_obj = new QT(data->parent);
    } else {
        qt_obj = new QT(nullptr);
    }
    T ctl;
    ctl.funcs = funcs;
    ctl.priv_data = (struct PUBase*)qt_obj;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename F, typename QT> T create_generic_func(F* funcs, void* priv_data) {
    QT* qt_obj = new QT();
    T ctl;
    ctl.funcs = funcs;
    ctl.priv_data = (struct PUBase*)qt_obj;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename QT> void destroy_generic(struct PUBase* qt_data) {
    QT* qt_obj = (QT*)qt_data;
    delete qt_obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUWidget create_widget(struct PUBase* priv_data) {
    return create_widget_func<struct PUWidget, struct PUWidgetFuncs, WRWidget>(&s_widget_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_widget(struct PUBase* priv_data) {
    destroy_generic<WRWidget>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUPushButton create_push_button(struct PUBase* priv_data) {
    return create_widget_func<struct PUPushButton, struct PUPushButtonFuncs, WRPushButton>(&s_push_button_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_push_button(struct PUBase* priv_data) {
    destroy_generic<WRPushButton>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUPainter create_painter(struct PUBase* priv_data) {
    return create_generic_func<struct PUPainter, struct PUPainterFuncs, QPainter>(&s_painter_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_painter(struct PUBase* priv_data) {
    destroy_generic<QPainter>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUListWidgetItem create_list_widget_item(struct PUBase* priv_data) {
    return create_generic_func<struct PUListWidgetItem, struct PUListWidgetItemFuncs, QListWidgetItem>(&s_list_widget_item_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_list_widget_item(struct PUBase* priv_data) {
    destroy_generic<QListWidgetItem>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUListWidget create_list_widget(struct PUBase* priv_data) {
    return create_widget_func<struct PUListWidget, struct PUListWidgetFuncs, WRListWidget>(&s_list_widget_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_list_widget(struct PUBase* priv_data) {
    destroy_generic<WRListWidget>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PULabel create_label(struct PUBase* priv_data) {
    return create_widget_func<struct PULabel, struct PULabelFuncs, WRLabel>(&s_label_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_label(struct PUBase* priv_data) {
    destroy_generic<WRLabel>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PULineEdit create_line_edit(struct PUBase* priv_data) {
    return create_widget_func<struct PULineEdit, struct PULineEditFuncs, WRLineEdit>(&s_line_edit_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_line_edit(struct PUBase* priv_data) {
    destroy_generic<WRLineEdit>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUPlainTextEdit create_plain_text_edit(struct PUBase* priv_data) {
    return create_widget_func<struct PUPlainTextEdit, struct PUPlainTextEditFuncs, WRPlainTextEdit>(&s_plain_text_edit_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_plain_text_edit(struct PUBase* priv_data) {
    destroy_generic<WRPlainTextEdit>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUSlider create_slider(struct PUBase* priv_data) {
    return create_widget_func<struct PUSlider, struct PUSliderFuncs, WRSlider>(&s_slider_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_slider(struct PUBase* priv_data) {
    destroy_generic<WRSlider>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUMainWindow create_main_window(struct PUBase* priv_data) {
    return create_widget_func<struct PUMainWindow, struct PUMainWindowFuncs, WRMainWindow>(&s_main_window_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_main_window(struct PUBase* priv_data) {
    destroy_generic<WRMainWindow>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUToolWindowManager create_tool_window_manager(struct PUBase* priv_data) {
    return create_widget_func<struct PUToolWindowManager, struct PUToolWindowManagerFuncs, WRToolWindowManager>(&s_tool_window_manager_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_tool_window_manager(struct PUBase* priv_data) {
    destroy_generic<WRToolWindowManager>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUFramelessWindow create_frameless_window(struct PUBase* priv_data) {
    return create_widget_func<struct PUFramelessWindow, struct PUFramelessWindowFuncs, WRFramelessWindow>(&s_frameless_window_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_frameless_window(struct PUBase* priv_data) {
    destroy_generic<WRFramelessWindow>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUAction create_action(struct PUBase* priv_data) {
    return create_generic_func<struct PUAction, struct PUActionFuncs, QAction>(&s_action_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_action(struct PUBase* priv_data) {
    destroy_generic<QAction>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUTimer create_timer(struct PUBase* priv_data) {
    return create_generic_func<struct PUTimer, struct PUTimerFuncs, QTimer>(&s_timer_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_timer(struct PUBase* priv_data) {
    destroy_generic<QTimer>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUIcon create_icon(struct PUBase* priv_data) {
    return create_generic_func<struct PUIcon, struct PUIconFuncs, QIcon>(&s_icon_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_icon(struct PUBase* priv_data) {
    destroy_generic<QIcon>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUFont create_font(struct PUBase* priv_data) {
    return create_generic_func<struct PUFont, struct PUFontFuncs, QFont>(&s_font_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_font(struct PUBase* priv_data) {
    destroy_generic<QFont>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUMenu create_menu(struct PUBase* priv_data) {
    return create_widget_func<struct PUMenu, struct PUMenuFuncs, WRMenu>(&s_menu_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_menu(struct PUBase* priv_data) {
    destroy_generic<WRMenu>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUMenuBar create_menu_bar(struct PUBase* priv_data) {
    return create_widget_func<struct PUMenuBar, struct PUMenuBarFuncs, WRMenuBar>(&s_menu_bar_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_menu_bar(struct PUBase* priv_data) {
    destroy_generic<WRMenuBar>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUVBoxLayout create_v_box_layout(struct PUBase* priv_data) {
    return create_widget_func<struct PUVBoxLayout, struct PUVBoxLayoutFuncs, WRVBoxLayout>(&s_v_box_layout_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_v_box_layout(struct PUBase* priv_data) {
    destroy_generic<WRVBoxLayout>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUHBoxLayout create_h_box_layout(struct PUBase* priv_data) {
    return create_widget_func<struct PUHBoxLayout, struct PUHBoxLayoutFuncs, WRHBoxLayout>(&s_h_box_layout_funcs, priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_h_box_layout(struct PUBase* priv_data) {
    destroy_generic<WRHBoxLayout>(priv_data);
}


#include <QStyleFactory>
#include <DarkStyle.h>
#include <QFileDialog>
#include <QTextStream>
//#include <QSvgRenderer>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUApplication create_application(struct PUBase* priv_data) {
    static int argc = 0;
    QApplication* qt_obj = new QApplication(argc, 0);

    create_enum_mappings();

    struct PUApplication ctl;
    ctl.funcs = &s_application_funcs;
    ctl.priv_data = (struct PUBase*)qt_obj;
    return ctl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void destroy_application(struct PUBase* priv_data) {
    destroy_generic<QApplication>(priv_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_add_widget_item(struct PUBase* self_c, struct PUBase* item) {
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->addItem((QListWidgetItem*)item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_add_action_text(struct PUBase* self_c, const char* text) {
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->addAction(QString::fromLatin1(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_set_accept_drops(struct PUBase* self_c, bool state) {
    WRListWidget* qt_data = (WRListWidget*)self_c;
	qt_data->viewport()->setAcceptDrops(state);
	qt_data->setDragDropMode(QAbstractItemView::InternalMove);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_fill_rect_color(struct PUBase* self_c, struct PURect rect, struct PUColor color) {
    QPainter* qt_data = (QPainter*)self_c;
    qt_data->fillRect(QRect(rect.x, rect.y, rect.width, rect.height), QColor(color.r, color.g, color.b, color.a));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_item_set_string_data(struct PUBase* self_c, const char* text) {
    QListWidgetItem* qt_data = (QListWidgetItem*)self_c;
    qt_data->setData(Qt::UserRole, QString::fromLatin1(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* list_widget_item_get_string_data(struct PUBase* self_c) {
    QListWidgetItem* qt_data = (QListWidgetItem*)self_c;
    auto ret_value = qt_data->data(Qt::UserRole).toString();
    QByteArray ba = ret_value.toUtf8();
    const char* c_str = ba.data();
    assert((ba.size() + 1) < sizeof(s_temp_string_buffer));
    memcpy(s_temp_string_buffer, c_str, ba.size() + 1);
    return s_temp_string_buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void list_widget_add_text_item(struct PUBase* self_c, const char* text) {
    WRListWidget* qt_data = (WRListWidget*)self_c;
    qt_data->addItem(QString::fromLatin1(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUArray application_get_files(struct PUBase* self_c) {
   QApplication* qt_data = (QApplication*)self_c;
   PUArray array = { 0 };
   (void)qt_data;
   QFileDialog dialog(QApplication::activeWindow());
   dialog.setFileMode(QFileDialog::ExistingFiles);
   dialog.show();

   if (!dialog.exec())
		return array;

   auto ret_value = dialog.selectedFiles();

   int count = ret_value.size();

   if (count > 0) {
       PUUrl* elements = new PUUrl[count];
       for (int i = 0; i < count; ++i) {
           elements[i].funcs = &s_url_funcs;
           QUrl* temp = new QUrl(QUrl::fromLocalFile(ret_value.at(i)));
           elements[i].priv_data = (struct PUBase*)temp;
      }

      array.elements = (void*)elements;
      array.count = int(count);
  }

   return array;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
static void icon_add_file(struct PUBase* self_c, const char* filename) {
    QIcon* qt_data = (QIcon*)self_c;
    QSvgRenderer svgRenderer(QString::fromLatin1(filename));
    printf("filling with svg renderer\n");
    // create pixmap target (could be a QImage)
    QPixmap pix(svgRenderer.defaultSize());
    pix.fill(Qt::transparent);
    // create painter to act over pixmap
    QPainter pixPainter(&pix);
    // use renderer to render over painter which paints on pixmap
    svgRenderer.render(&pixPainter);
    //QIcon myicon(pix);
    qt_data->addPixmap(pix);
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void action_set_int_data(struct PUBase* self_c, int data) {
    QAction* qt_data = (QAction*)self_c;
    qt_data->setData(QVariant(data));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int action_get_int_data(struct PUBase* self_c) {
    QAction* qt_data = (QAction*)self_c;
    return qt_data->data().toInt();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void action_set_shortcut(struct PUBase* self_c, PUKeys key) {
    QAction* qt_data = (QAction*)self_c;
    qt_data->setShortcut(s_keys_lookup[(int)key]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void action_set_shortcut_mod(struct PUBase* self_c, PUKeys key, PUMetaKeys modifier) {
    QAction* qt_data = (QAction*)self_c;
    int tkey = s_keys_lookup[(int)key];
    int tmod = s_meta_keys_lookup[(int)modifier];

    qt_data->setShortcut(tkey + tmod);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tool_window_manager_add_to_docking(struct PUBase* self_c, struct PUBase* widget) {
    WRToolWindowManager* qt_data = (WRToolWindowManager*)self_c;
    ToolWindowManager::AreaReferenceType type;

    static int hack = 0;

    if (hack == 0) {
        type = ToolWindowManager::EmptySpace;
        hack = 1;
    } else {
        type = ToolWindowManager::LastUsedArea;
    }

    qt_data->addToolWindow((QWidget*)widget, type);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int application_set_style_sheet(struct PUBase* self_c, const char* filename) {
    QApplication* qt_data = (QApplication*)self_c;
    QFile f(QString::fromUtf8(filename));

    if (!f.exists()) {
        printf("Unable to set stylesheet: %s, file not found\n", filename);
        return 0;
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qt_data->setStyleSheet(ts.readAll());
    }

    return 1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUWidgetFuncs s_widget_funcs = {
    destroy_widget,
    widget_show,
    widget_set_fixed_height,
    widget_set_fixed_width,
    widget_resize,
    widget_set_parent,
    widget_set_layout,
    widget_update,
    set_widget_paint_event,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUPushButtonFuncs s_push_button_funcs = {
    destroy_push_button,
    push_button_show,
    push_button_set_fixed_height,
    push_button_set_fixed_width,
    push_button_resize,
    push_button_set_parent,
    push_button_set_layout,
    push_button_update,
    set_push_button_pressed_event,
    set_push_button_released_event,
    push_button_set_icon,
    push_button_set_text,
    push_button_set_flat,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUPainterFuncs s_painter_funcs = {
    destroy_painter,
    painter_begin,
    painter_end,
    painter_set_font,
    painter_draw_text,
    painter_draw_line,
    painter_fill_rect_color,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUListWidgetItemFuncs s_list_widget_item_funcs = {
    destroy_list_widget_item,
    list_widget_item_set_text,
    list_widget_item_text,
    list_widget_item_set_string_data,
    list_widget_item_get_string_data,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUListWidgetFuncs s_list_widget_funcs = {
    destroy_list_widget,
    list_widget_show,
    list_widget_set_fixed_height,
    list_widget_set_fixed_width,
    list_widget_resize,
    list_widget_set_parent,
    list_widget_set_layout,
    list_widget_update,
    list_widget_add_item,
    list_widget_add_text_item,
    list_widget_current_item,
    list_widget_current_row,
    list_widget_selected_items,
    list_widget_item,
    list_widget_set_current_row,
    list_widget_count,
    list_widget_set_drag_enabled,
    list_widget_set_drop_indicator_shown,
    list_widget_set_accept_drops,
    list_widget_add_widget_item,
    set_list_widget_current_row_changed_event,
    set_list_widget_item_clicked_event,
    set_list_widget_item_double_clicked_event,
    set_list_widget_drag_enter_event,
    set_list_widget_drop_event,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PULabelFuncs s_label_funcs = {
    destroy_label,
    label_show,
    label_set_fixed_height,
    label_set_fixed_width,
    label_resize,
    label_set_parent,
    label_set_layout,
    label_update,
    label_set_text,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PULineEditFuncs s_line_edit_funcs = {
    destroy_line_edit,
    line_edit_show,
    line_edit_set_fixed_height,
    line_edit_set_fixed_width,
    line_edit_resize,
    line_edit_set_parent,
    line_edit_set_layout,
    line_edit_update,
    line_edit_set_text,
    line_edit_set_read_only,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUPlainTextEditFuncs s_plain_text_edit_funcs = {
    destroy_plain_text_edit,
    plain_text_edit_show,
    plain_text_edit_set_fixed_height,
    plain_text_edit_set_fixed_width,
    plain_text_edit_resize,
    plain_text_edit_set_parent,
    plain_text_edit_set_layout,
    plain_text_edit_update,
    plain_text_edit_clear,
    plain_text_edit_set_plain_text,
    plain_text_edit_append_plain_text,
    plain_text_edit_set_read_only,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUSliderFuncs s_slider_funcs = {
    destroy_slider,
    slider_show,
    slider_set_fixed_height,
    slider_set_fixed_width,
    slider_resize,
    slider_set_parent,
    slider_set_layout,
    slider_update,
    set_slider_value_changed_event,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUMainWindowFuncs s_main_window_funcs = {
    destroy_main_window,
    main_window_show,
    main_window_set_fixed_height,
    main_window_set_fixed_width,
    main_window_resize,
    main_window_set_parent,
    main_window_set_layout,
    main_window_update,
    main_window_is_animated,
    main_window_menu_bar,
    main_window_set_central_widget,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUToolWindowManagerFuncs s_tool_window_manager_funcs = {
    destroy_tool_window_manager,
    tool_window_manager_show,
    tool_window_manager_set_fixed_height,
    tool_window_manager_set_fixed_width,
    tool_window_manager_resize,
    tool_window_manager_set_parent,
    tool_window_manager_set_layout,
    tool_window_manager_update,
    tool_window_manager_add_to_docking,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUFramelessWindowFuncs s_frameless_window_funcs = {
    destroy_frameless_window,
    frameless_window_show,
    frameless_window_set_fixed_height,
    frameless_window_set_fixed_width,
    frameless_window_resize,
    frameless_window_set_parent,
    frameless_window_set_layout,
    frameless_window_update,
    frameless_window_set_window_title,
    frameless_window_set_content,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUActionFuncs s_action_funcs = {
    destroy_action,
    action_is_enabled,
    action_set_text,
    action_set_shortcut,
    action_set_shortcut_mod,
    set_action_triggered_event,
    action_set_int_data,
    action_get_int_data,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUUrlFuncs s_url_funcs = {
    url_is_local_file,
    url_to_local_file,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUMimeDataFuncs s_mime_data_funcs = {
    mime_data_has_color,
    mime_data_has_image,
    mime_data_has_text,
    mime_data_has_urls,
    mime_data_urls,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUTimerFuncs s_timer_funcs = {
    destroy_timer,
    set_timer_timeout_event,
    timer_start,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUIconFuncs s_icon_funcs = {
    destroy_icon,
    icon_add_file,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUFontFuncs s_font_funcs = {
    destroy_font,
    font_set_family,
    font_set_point_size,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUMenuFuncs s_menu_funcs = {
    destroy_menu,
    menu_show,
    menu_set_fixed_height,
    menu_set_fixed_width,
    menu_resize,
    menu_set_parent,
    menu_set_layout,
    menu_update,
    menu_add_action_text,
    set_menu_triggered_event,
    menu_add_action,
    menu_set_title,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUMenuBarFuncs s_menu_bar_funcs = {
    destroy_menu_bar,
    menu_bar_show,
    menu_bar_set_fixed_height,
    menu_bar_set_fixed_width,
    menu_bar_resize,
    menu_bar_set_parent,
    menu_bar_set_layout,
    menu_bar_update,
    menu_bar_add_menu,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUApplicationFuncs s_application_funcs = {
    destroy_application,
    application_set_style,
    application_set_style_sheet,
    application_exec,
    set_application_about_to_quit_event,
    application_get_files,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUPaintEventFuncs s_paint_event_funcs = {
    paint_event_rect,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUDragEnterEventFuncs s_drag_enter_event_funcs = {
    drag_enter_event_accept,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUDropEventFuncs s_drop_event_funcs = {
    drop_event_accept_proposed_action,
    drop_event_mime_data,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PULayoutFuncs s_layout_funcs = {
    layout_add_widget,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUVBoxLayoutFuncs s_v_box_layout_funcs = {
    destroy_v_box_layout,
    v_box_layout_add_widget,
    v_box_layout_add_layout,
    v_box_layout_update,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUHBoxLayoutFuncs s_h_box_layout_funcs = {
    destroy_h_box_layout,
    h_box_layout_add_widget,
    h_box_layout_add_layout,
    h_box_layout_update,
};

static struct PUPluginUI s_plugin_ui = {
    create_widget,
    create_push_button,
    create_painter,
    create_list_widget_item,
    create_list_widget,
    create_label,
    create_line_edit,
    create_plain_text_edit,
    create_slider,
    create_main_window,
    create_frameless_window,
    create_action,
    create_timer,
    create_icon,
    create_font,
    create_menu,
    create_menu_bar,
    create_v_box_layout,
    create_h_box_layout,
    0,

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct PUPluginUI* create_plugin_ui(struct PUBase* user_data, struct PUBase* parent) {
    struct PUPluginUI* instance = new PUPluginUI;
    memcpy(instance, &s_plugin_ui, sizeof(PUPluginUI));
    PrivData* priv_data = new PrivData;
    priv_data->parent = (QWidget*)parent;
    instance->priv_data = (PUBase*)priv_data;
    return instance;
}

void destroy_plugin_ui(PUPluginUI* plugin_ui) {
    PrivData* priv_data = (PrivData*)plugin_ui->priv_data;
    delete priv_data;
    delete plugin_ui;
}


static struct PU s_pu = {
    create_widget,
    create_push_button,
    create_painter,
    create_list_widget_item,
    create_list_widget,
    create_label,
    create_line_edit,
    create_plain_text_edit,
    create_slider,
    create_main_window,
    create_tool_window_manager,
    create_frameless_window,
    create_action,
    create_timer,
    create_icon,
    create_font,
    create_menu,
    create_menu_bar,
    create_application,
    create_v_box_layout,
    create_h_box_layout,
    0,    create_plugin_ui,
    destroy_plugin_ui,
    0,

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
extern "C" __declspec(dllexport) struct PU* wrui_get() {
#else
extern "C" struct PU* wrui_get() {
#endif
    return (PU*)&s_pu;
}
