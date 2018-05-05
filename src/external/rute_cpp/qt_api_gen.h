#pragma once

#include "c_api.h"
#include <QObject>
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct RUWidgetFuncs s_widget_funcs;
extern struct RUPushButtonFuncs s_push_button_funcs;
extern struct RUPainterFuncs s_painter_funcs;
extern struct RUListWidgetItemFuncs s_list_widget_item_funcs;
extern struct RUListWidgetFuncs s_list_widget_funcs;
extern struct RULabelFuncs s_label_funcs;
extern struct RULineEditFuncs s_line_edit_funcs;
extern struct RUPlainTextEditFuncs s_plain_text_edit_funcs;
extern struct RUSliderFuncs s_slider_funcs;
extern struct RUMainWindowFuncs s_main_window_funcs;
extern struct RUToolWindowManagerFuncs s_tool_window_manager_funcs;
extern struct RUFramelessWindowFuncs s_frameless_window_funcs;
extern struct RUActionFuncs s_action_funcs;
extern struct RUUrlFuncs s_url_funcs;
extern struct RUMimeDataFuncs s_mime_data_funcs;
extern struct RUTimerFuncs s_timer_funcs;
extern struct RUIconFuncs s_icon_funcs;
extern struct RUFontFuncs s_font_funcs;
extern struct RUMenuFuncs s_menu_funcs;
extern struct RUMenuBarFuncs s_menu_bar_funcs;
extern struct RUApplicationFuncs s_application_funcs;
extern struct RUPaintEventFuncs s_paint_event_funcs;
extern struct RUDragEnterEventFuncs s_drag_enter_event_funcs;
extern struct RUDropEventFuncs s_drop_event_funcs;
extern struct RULayoutFuncs s_layout_funcs;
extern struct RUVBoxLayoutFuncs s_v_box_layout_funcs;
extern struct RUHBoxLayoutFuncs s_h_box_layout_funcs;

#include <QAction>
#include <QListWidgetItem>

extern struct RUActionFuncs s_action_funcs;
extern struct RUListWidgetItemFuncs s_list_widget_item_funcs;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*Signal_self_i32_void)(void* self_c, int row);

class QSlotWrapperSignal_self_i32_void : public QObject {
    Q_OBJECT
public:
    QSlotWrapperSignal_self_i32_void(void* data, Signal_self_i32_void func) {
        m_func = func;
        m_data = data;
    }

    Q_SLOT void method(int row) {
        m_func(m_data, row);
    }
private:
    Signal_self_i32_void m_func;
    void* m_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*Signal_self_Action_void)(void* self_c, struct RUBase* action);

class QSlotWrapperSignal_self_Action_void : public QObject {
    Q_OBJECT
public:
    QSlotWrapperSignal_self_Action_void(void* data, Signal_self_Action_void func) {
        m_func = func;
        m_data = data;
    }

    Q_SLOT void method(QAction* action) {
        auto temp_arg_1 = RUAction { &s_action_funcs, (struct RUBase*)action };
        m_func(m_data, (struct RUBase*)&temp_arg_1);
    }
private:
    Signal_self_Action_void m_func;
    void* m_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*Signal_self_ListWidgetItem_void)(void* self_c, struct RUBase* item);

class QSlotWrapperSignal_self_ListWidgetItem_void : public QObject {
    Q_OBJECT
public:
    QSlotWrapperSignal_self_ListWidgetItem_void(void* data, Signal_self_ListWidgetItem_void func) {
        m_func = func;
        m_data = data;
    }

    Q_SLOT void method(QListWidgetItem* item) {
        auto temp_arg_1 = RUListWidgetItem { &s_list_widget_item_funcs, (struct RUBase*)item };
        m_func(m_data, (struct RUBase*)&temp_arg_1);
    }
private:
    Signal_self_ListWidgetItem_void m_func;
    void* m_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*Signal_self_void)(void* self_c);

class QSlotWrapperSignal_self_void : public QObject {
    Q_OBJECT
public:
    QSlotWrapperSignal_self_void(void* data, Signal_self_void func) {
        m_func = func;
        m_data = data;
    }

    Q_SLOT void method() {
        m_func(m_data);
    }
private:
    Signal_self_void m_func;
    void* m_data;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRWidget : public QWidget {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRWidget(QWidget* widget) : QWidget(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRWidget() {}

public:
    virtual void paintEvent(QPaintEvent* event);
    void (*m_paint)(void* self_c, struct RUBase* event) = nullptr;
    void* m_paint_user_data = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRPushButton : public QPushButton {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRPushButton(QWidget* widget) : QPushButton(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRPushButton() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRListWidget : public QListWidget {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRListWidget(QWidget* widget) : QListWidget(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRListWidget() {}

public:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    void (*m_drag_enter)(void* self_c, struct RUBase* event) = nullptr;
    void* m_drag_enter_user_data = nullptr;
public:
    virtual void dropEvent(QDropEvent* event);
    void (*m_drop)(void* self_c, struct RUBase* event) = nullptr;
    void* m_drop_user_data = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRLabel : public QLabel {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRLabel(QWidget* widget) : QLabel(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRLabel() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRLineEdit : public QLineEdit {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRLineEdit(QWidget* widget) : QLineEdit(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRLineEdit() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRPlainTextEdit(QWidget* widget) : QPlainTextEdit(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRPlainTextEdit() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRSlider : public QSlider {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRSlider(QWidget* widget) : QSlider(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRSlider() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMainWindow : public QMainWindow {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRMainWindow(QWidget* widget) : QMainWindow(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRMainWindow() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRToolWindowManager : public QToolWindowManager {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRToolWindowManager(QWidget* widget) : QToolWindowManager(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRToolWindowManager() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRFramelessWindow : public QFramelessWindow {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRFramelessWindow(QWidget* widget) : QFramelessWindow(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRFramelessWindow() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMenu : public QMenu {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRMenu(QWidget* widget) : QMenu(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRMenu() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRMenuBar : public QMenuBar {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRMenuBar(QWidget* widget) : QMenuBar(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRMenuBar() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRVBoxLayout : public QVBoxLayout {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRVBoxLayout(QWidget* widget) : QVBoxLayout(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRVBoxLayout() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WRHBoxLayout : public QHBoxLayout {
    Q_OBJECT
public:
    Q_PROPERTY(QString persistData READ persistData WRITE setPersistData DESIGNABLE false SCRIPTABLE false)
    void setPersistData(const QString& data) { m_persistData = data; }
    QString persistData() { return m_persistData; }
    QString m_persistData;

    WRHBoxLayout(QWidget* widget) : QHBoxLayout(widget) {  setObjectName(QStringLiteral("Test")); setPersistData(QStringLiteral("SomeData")); }
    virtual ~WRHBoxLayout() {}

};

