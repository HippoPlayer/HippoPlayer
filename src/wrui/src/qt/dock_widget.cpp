#include "../../include/wrui.h"
#include "../../include/dock_widget.h"
#include "widget_private.h"
#include <QDockWidget>

/*

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct GUDockWidget* create(struct GUWidget* parent) {
	QDockWidget* qdock = nullptr;

	if (parent)
		qdock = new QDockWidget((QWidget*)parent->object->p);
	else
		qdock = new QDockWidget(nullptr);

	GUDockWidget* t = new GUDockWidget;
	t->priv = 0;

	widget_setup(t->base, (void*) qdock);

	return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct GUDockWidget* create_title(const char* name, struct GUWidget* parent) {
	(void)name;
	printf("dock_widget: create_title not fully implemented yet\n");
	return create(parent);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool is_floating(GUDockWidget* w) {
	return ((QDockWidget*)w->base->object->p)->isFloating();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_floating(GUDockWidget* w, bool state) {
	((QDockWidget*)w->base->object->p)->setFloating(state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void set_widget(GUDockWidget* dock, GUWidget* widget) {
	dock->priv = (void*)widget;
	((QDockWidget*)dock->base->object->p)->setWidget((QWidget*)widget->object->p);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct GUWidget* widget(GUDockWidget* w) {
	return (GUWidget*)w->priv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GUDockWidgetFuncs s_dockWidgetFuncs = {
	create,
	create_title,
	is_floating,
	set_floating,
	set_widget,
	widget,
};

*/
