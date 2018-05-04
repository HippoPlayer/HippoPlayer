#pragma once

#include "c_api.h"
#include <QObject>
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

