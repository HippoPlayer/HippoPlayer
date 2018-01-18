#include "c_api.h"
#include "qt_api_gen.h"
#include <assert.h>
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QListWidgetItem>
#include <QListWidget>
#include <QSlider>
#include <QMainWindow>
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
    void* user_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct PUWidgetFuncs s_widget_funcs;
extern struct PUPushButtonFuncs s_push_button_funcs;
extern struct PUPainterFuncs s_painter_funcs;
extern struct PUListWidgetItemFuncs s_list_widget_item_funcs;
extern struct PUListWidgetFuncs s_list_widget_funcs;
extern struct PUSliderFuncs s_slider_funcs;
extern struct PUMainWindowFuncs s_main_window_funcs;
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
    qt_data->setText(QString::fromLatin1(text));
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
    qt_data->drawText(x, y, QString::fromLatin1(text));
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
    qt_data->setText(QString::fromLatin1(text));
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
    qt_data->setWindowTitle(QString::fromLatin1(title));
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
    qt_data->setText(QString::fromLatin1(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_action_triggered_event(void* object, void* user_data, void (*event)(void* self_c)) {
    QSlotWrapperSignal_self_void* wrap = new QSlotWrapperSignal_self_void(user_data, (Signal_self_void)event);
    QObject* q_obj = (QObject*)object;
    QObject::connect(q_obj, SIGNAL(triggered()), wrap, SLOT(method()));
}

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
    qt_data->addFile(QString::fromLatin1(filename));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void font_set_family(struct PUBase* self_c, const char* family) { 
    QFont* qt_data = (QFont*)self_c;
    qt_data->setFamily(QString::fromLatin1(family));
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

static void menu_add_action(struct PUBase* self_c, struct PUBase* action) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->addAction((QAction*)action);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void menu_set_title(struct PUBase* self_c, const char* title) { 
    WRMenu* qt_data = (WRMenu*)self_c;
    qt_data->setTitle(QString::fromLatin1(title));
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
    qt_data->setStyle(QString::fromLatin1(style));
}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct PUApplication create_application(struct PUBase* priv_data) {
    static int argc = 0;
    QApplication* qt_obj = new QApplication(argc, 0);

    //QGuiApplication::setOrganizationName(QStringLiteral("TBL"));
    //QCoreApplication::setOrganizationDomain(QStringLiteral("tbl.org"));

    qt_obj->setStyle(new DarkStyle);

    /*
    qt_obj->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, QColor(170,170,170));
    darkPalette.setColor(QPalette::Text, QColor(170,170,170));
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(50, 60, 70));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qt_obj->setPalette(darkPalette);
    */

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
    (void)qt_data;
    auto ret_value = QFileDialog::getOpenFileUrls();
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

struct PUWidgetFuncs s_widget_funcs = {
    destroy_widget,
    widget_show,
    widget_set_fixed_height,
    widget_set_fixed_width,
    widget_resize,
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

struct PUSliderFuncs s_slider_funcs = {
    destroy_slider,
    slider_show,
    slider_set_fixed_height,
    slider_set_fixed_width,
    slider_resize,
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
    main_window_set_layout,
    main_window_update,
    main_window_is_animated,
    main_window_menu_bar,
    main_window_set_central_widget,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUFramelessWindowFuncs s_frameless_window_funcs = {
    destroy_frameless_window,
    frameless_window_show,
    frameless_window_set_fixed_height,
    frameless_window_set_fixed_width,
    frameless_window_resize,
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
    set_action_triggered_event,
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
    menu_set_layout,
    menu_update,
    menu_add_action_text,
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
    menu_bar_set_layout,
    menu_bar_update,
    menu_bar_add_menu,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUApplicationFuncs s_application_funcs = {
    destroy_application,
    application_set_style,
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
    v_box_layout_update,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PUHBoxLayoutFuncs s_h_box_layout_funcs = {
    destroy_h_box_layout,
    h_box_layout_add_widget,
    h_box_layout_update,
};

static struct PU s_pu = {
    create_widget,
    create_push_button,
    create_painter,
    create_list_widget_item,
    create_list_widget,
    create_slider,
    create_main_window,
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
    0,

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct PU* PU_create_instance(void* user_data, QWidget* parent) {
    struct PU* instance = new PU;
    memcpy(instance, &s_pu, sizeof(PU));
    PrivData* priv_data = new PrivData;
    priv_data->parent = parent;
    priv_data->user_data = user_data;
    return instance;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" struct PU* wrui_get() {
    return (PU*)&s_pu;
}
