#include "../../include/wrui.h"
#include "signal_wrappers.h"
#include "widget_private.h"
#include <QApplication>
#include <QPushButton>
#include <QMainWindow>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline QColor from(WUColor color) {
    return QColor(int(color.r * 255.0f), int(color.g * 255.0f), int(color.b * 255.0f), int(color.a * 255.0f));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline QRect from(WURect rect) {
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

#if 0

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int connect(void* sender, const char* id, void* reciver, void* func) {
	GUObject* object = (GUObject*)sender;
	QObject* q_obj = (QObject*)object->p;

	QSlotWrapperNoArgs* wrap = new QSlotWrapperNoArgs(reciver, (SignalNoArgs)func);

	QObject::connect(q_obj, id, wrap, SLOT(method()));
	/*
		return 1;
	} else {
		printf("wrui: unable to create connection between (%p - %s) -> (%p -> %p)\n");
		return 0;
	}
	*/
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GUObjectFuncs s_objFuncs = {
	connect,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_size(struct GUWidget* widget, int width, int height) {
	QObject* q_obj = (QObject*) widget->object->p;
	QWidget* q_widget = static_cast<QWidget*>(q_obj);
	q_widget->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct GUWidgetFuncs s_widgetFuncs = {
	widget_set_size,
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUHandle main_window_create() {
    auto* window = new QMainWindow();
    window->show();
    return (WUHandle)window;
}

static WUMainWindowFuncs s_main_window_funcs = {
    main_window_create,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUHandle widget_button_create(WUHandle parent) {
    auto* button = new QPushButton((QWidget*)parent);
    return (WUHandle)button;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void widget_set_size(WUHandle handle, int width, int height) {
	auto* widget = (QWidget*)handle;
	widget->resize(width, height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUWidgetFuncs s_widget_funcs = {
    widget_button_create,
	widget_set_size,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct WUQPainter {
    WUPainter wu_painter;
    QPainter* q_painter;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_draw_text(const struct WUPainter* painter,
                              WUPos pos, WUColor color,
                              const char* text, int len, const WUFont* font) {
    WUQPainter* wu_painter = (WUQPainter*)painter;
    QPainter* p = wu_painter->q_painter;

    p->setPen(from(color));
    p->drawText((int)pos.x, (int)pos.y, QString::fromLatin1(text, len));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void painter_draw_rect(const struct WUPainter* painter, WURect rect, WUColor color) {
    WUQPainter* wu_painter = (WUQPainter*)painter;
    QPainter* p = wu_painter->q_painter;

    p->fillRect(from(rect), from(color));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUPainter s_paint_funcs = {
    nullptr,
    painter_draw_text,
    painter_draw_rect,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WUQWidget : public QWidget {
public:
    WUQWidget(QWidget* parent) : QWidget(parent) {}

    virtual void paintEvent(QPaintEvent* event) {
        QPainter q_painter(this);
        if (m_paint_event) {
            WUQPainter painter;
            painter.wu_painter = s_paint_funcs;
            painter.q_painter = &q_painter;
            m_paint_event((WUPainter*)&painter, m_paint_user_data);
        }
        else {
            QWidget::paintEvent(event);
        }
    }

    WUPaintEvent m_paint_event = nullptr;
    void* m_paint_user_data = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUHandle window_create(WUHandle parent) {
    auto* w = new WUQWidget((QWidget*)parent);
    w->show();
    return (WUHandle)w;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void window_set_paint_event(WUHandle handle, void* user_data, WUPaintEvent event) {
    auto* w = (WUQWidget*)handle;
    w->m_paint_event = event;
    w->m_paint_user_data = user_data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUWindowFuncs s_window_funcs {
    window_create,
    window_set_paint_event,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WUHandle app_create() {
	int argc = 0;
	auto app = new QApplication(argc, 0);
	return (WUHandle)app;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int app_run(WUHandle handle) {
	QApplication* qt_app = (QApplication*)handle;
	return qt_app->exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct WUApplicationFuncs s_app_funcs = {
    app_create,
	app_run,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Wrui s_wrui = {
    WRUI_VERSION,

	// user facing

	&s_app_funcs,
	&s_window_funcs,
	&s_main_window_funcs,
	&s_widget_funcs
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" Wrui* wrui_get() {
	return &s_wrui;
}
